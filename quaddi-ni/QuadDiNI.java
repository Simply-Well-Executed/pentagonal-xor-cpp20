// SPDX-License-Identifier: 0BSD
// QUADDI-NI independent Oracle Java 27.0.1.1 evaluator, no downloads/network.
import java.io.BufferedReader;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Locale;

public final class QuadDiNI {
    private static void hold(String reason) {
        throw new IllegalStateException("QUADDI|HOLD|" + reason);
    }
    private static BufferedReader reader(String path) throws IOException {
        return Files.newBufferedReader(Path.of(path), StandardCharsets.US_ASCII);
    }
    private static boolean ignored(String line) {
        return line.length()==0 || line.charAt(0)=='#';
    }
    private static void safeName(String name) {
        if(!name.matches("[A-Za-z0-9_.-]+")) hold("invalid_name");
    }
    private static double decimal(String field) {
        if(field.isEmpty()) hold("empty_number");
        try {
            double value=Double.parseDouble(field);
            if(!Double.isFinite(value)) hold("number_nonfinite");
            return value;
        } catch(NumberFormatException ex) { hold("invalid_number");return 0; }
    }
    private static long count(String file,boolean names) throws IOException {
        long result=0;
        try(BufferedReader r=reader(file)) {
            for(String line;(line=r.readLine())!=null;) {
                if(ignored(line))continue;
                if(!StandardCharsets.US_ASCII.newEncoder().canEncode(line))
                    hold("non_ASCII_input");
                if(names)safeName(line);
                if(result==Long.MAX_VALUE)hold("row_count_overflow");
                ++result;
            }
        }
        return result;
    }
    private static double power(double value,int n) {
        // Empty product: t^0=1 including t=0, by engine convention.
        double v=1,base=value;
        int exponent=n;
        while(exponent!=0) {
            if((exponent&1)!=0)v*=base;
            exponent>>>=1;
            if(exponent!=0)base*=base;
            if(!Double.isFinite(v)||!Double.isFinite(base))hold("power_nonfinite");
        }
        return v;
    }
    private static boolean prime(long n) {
        if(n<2)return false;
        if(n%2==0)return n==2;
        for(long k=3;k<=n/k;k+=2)if(n%k==0)return false;
        return true;
    }
    private static long primeDegree(double origin) {
        double magnitude=Math.abs(origin);
        if(magnitude>9007199254740991.0)hold("prime_degree_not_exactly_representable");
        long center=(long)Math.floor(magnitude+0.5);
        for(long d=0;;++d) {
            long lower=d<=center?center-d:0,upper=center+d;
            boolean l=prime(lower),h=prime(upper);
            if(l||h) {
                if(l&&h&&lower!=upper)
                    return Math.abs(magnitude-lower)<=Math.abs(upper-magnitude)?lower:upper;
                return l?lower:upper;
            }
            if(d>=Long.MAX_VALUE-center)hold("prime_search_overflow");
        }
    }
    private record Witness(double[] a,double[] b,double[] gamma,double closure,long degree) {}
    private static Witness witness(double intercept,int power) {
        double origin=power(intercept,power);
        long degree=primeDegree(origin);
        double magnitude=Math.pow(Math.abs(origin),1.0/degree);
        if(!Double.isFinite(magnitude))hold("prime_root_nonfinite");
        double rr=0,ri=0;
        if(origin<0&&degree==2)ri=magnitude;
        else rr=origin<0?-magnitude:magnitude;
        double pi=3.141592653589793;
        double[] a={0,0,rr,rr+pi,rr+pi,rr};
        double[] b={0,-1,ri-1,ri-1,ri,ri};
        double[] gamma=new double[6];
        for(int i=1;i<6;i++)
            gamma[i]=Math.hypot(a[i]-a[i-1],b[i]-b[i-1]);
        return new Witness(a,b,gamma,Math.hypot(rr,ri),degree);
    }
    private static void row(long page,long ordinal,String name,int power,
                            int state,double x,double re,double im,
                            double cx,Witness w) {
        if(!Double.isFinite(re)||!Double.isFinite(im))hold("computed_state_nonfinite");
        String c=Double.isFinite(cx)?String.format(Locale.ROOT,"%.17g",cx):"NA";
        System.out.printf(Locale.ROOT,
            "ROW\t%d\t%d\t%s\t%d\t%d\t%.17g\t%.17g\t%.17g\t%s\t%.17g\t%.17g\t%d%n",
            page,ordinal,name,power,state,x,re,im,c,w.gamma()[state-1],w.closure(),w.degree());
    }
    private static void samples(String file,String name,long ordinal,long notes,
                                double slope,double intercept,int p) throws IOException {
        Witness w=witness(intercept,p);
        double cx=slope==0?Double.POSITIVE_INFINITY:-intercept/slope;
        long seen=0;
        try(BufferedReader r=reader(file)) {
            for(String line;(line=r.readLine())!=null;) {
                if(ignored(line))continue;
                double x=decimal(line);
                double t=slope*x+intercept;
                double core=power(t,p);
                for(int i=0;i<6;i++)row(i+1,ordinal,name,p,i+1,x,
                                          core+w.a()[i],w.b()[i],cx,w);
                row(6+notes+ordinal,ordinal,name,p,6,x,core+w.a()[5],w.b()[5],cx,w);
                seen++;
            }
        }
        if(seen==0)hold("empty_sample_file");
    }
    public static void main(String[] args) throws Exception {
        if(args.length!=3 && args.length!=4)hold("usage_lines_samples_optional_power_notes");
        if(!System.getProperty("java.vendor").equals("Oracle Corporation")||
           !System.getProperty("java.version").equals("27.0.1.1"))
            hold("exact_Oracle_27.0.1.1_required");
        int p=0;
        if(args.length==4) {
            if(!args[2].matches("[0-9]+"))hold("nonnegative_power_required");
            try {p=Integer.parseInt(args[2]);}
            catch(NumberFormatException e){hold("power_not_representable");return;}
        }
        String notesFile=args[args.length-1];
        long lines=count(args[0],false),notes=count(notesFile,true);
        if(lines==0||notes>Long.MAX_VALUE-6-lines)hold("page_count_overflow_or_no_lines");
        System.out.printf(Locale.ROOT,"META\t1\t6\t%d\t%d\t%d\t%d%n",
                          lines,notes,6+lines+notes,p);
        long noteIndex=0;
        try(BufferedReader r=reader(notesFile)) {
            for(String line;(line=r.readLine())!=null;) {
                if(ignored(line))continue;
                safeName(line);
                System.out.printf(Locale.ROOT,"NOTE\t%d\t%s%n",7+noteIndex,line);
                noteIndex++;
            }
        }
        long ordinal=0;
        try(BufferedReader r=reader(args[0])) {
            for(String line;(line=r.readLine())!=null;) {
                if(ignored(line))continue;
                if(!StandardCharsets.US_ASCII.newEncoder().canEncode(line))
                    hold("non_ASCII_input");
                String[] fields=line.split("\\|",-1);
                if(fields.length!=3)hold("line_must_have_three_fields");
                safeName(fields[0]);
                double slope=decimal(fields[1]),intercept=decimal(fields[2]);
                samples(args[1],fields[0],++ordinal,notes,slope,intercept,p);
            }
        }
        System.out.printf(Locale.ROOT,"END\t%d\t%d\t%d\tRELEASE=HOLD%n",
                          lines,6+lines+notes,p);
    }
}
