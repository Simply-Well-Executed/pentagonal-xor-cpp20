// SPDX-License-Identifier: 0BSD
// Independent Oracle Java 27.0.1.1 witness for SLIDINGTOOLONGA-001.
// Exact finite decimal seed at x=0. No double/float, external calls or networks.
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.nio.charset.CodingErrorAction;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.math.BigInteger;
import java.util.Locale;

public final class SlidingToolongA {
    private static void hold(String why) {
        throw new IllegalStateException("SLIDINGTOOLONGA|HOLD|" + why);
    }
    private record Decimal(BigInteger mantissa, int scale, int sign) {}
    private static Decimal decimal(String s) {
        if(!s.matches("[+-]?(?:[0-9]+(?:\\.[0-9]*)?|\\.[0-9]+)")) {
            hold("invalid_decimal");
        }
        int sign=1;
        if(s.startsWith("-")){sign=-1;s=s.substring(1);}
        else if(s.startsWith("+"))s=s.substring(1);
        int dot=s.indexOf('.');
        int scale=dot<0?0:s.length()-dot-1;
        String digits=s.replace(".","");
        BigInteger mag=new BigInteger(digits);
        return new Decimal(mag,scale,mag.signum()==0?0:sign);
    }
    private static BigInteger exponent(String s) {
        if(!s.matches("[0-9]+"))hold("invalid_exponent");
        return new BigInteger(s);
    }
    private static BigInteger power(BigInteger mantissa,BigInteger exponent) {
        BigInteger out=BigInteger.ONE,base=mantissa;
        while(exponent.signum()>0) {
            if(exponent.testBit(0))out=out.multiply(base);
            exponent=exponent.shiftRight(1);
            if(exponent.signum()>0)base=base.multiply(base);
        }
        return out;
    }
    public static void main(String[] args) throws Exception {
        if(args.length!=1&&args.length!=2)hold("usage_lines_optional_exponent");
        if(!System.getProperty("java.vendor").equals("Oracle Corporation")||
           !System.getProperty("java.version").equals("27.0.1.1"))
            hold("Oracle_27.0.1.1_required");
        String exponentText=args.length==2?args[1]:"0";
        BigInteger p=exponent(exponentText);
        System.out.printf(Locale.ROOT,
            "META\tSLIDINGTOOLONGA-001\tPOWER=%s\tX=0\tEXACT_DECIMAL%n",exponentText);
        long count=0;
        var decoder=StandardCharsets.US_ASCII.newDecoder()
            .onMalformedInput(CodingErrorAction.REPORT)
            .onUnmappableCharacter(CodingErrorAction.REPORT);
        try(BufferedReader reader=new BufferedReader(
            new InputStreamReader(Files.newInputStream(Path.of(args[0])),decoder))) {
            for(String line;(line=reader.readLine())!=null;) {
                if(line.isEmpty()||line.charAt(0)=='#')continue;
                String[] cells=line.split("\\|",-1);
                if(cells.length!=3||!cells[0].matches("[A-Za-z0-9_.-]+"))
                    hold("unsafe_identity_or_fields");
                Decimal slope=decimal(cells[1]);
                Decimal intercept=decimal(cells[2]);
                // Both named source values are independently parsed and preserved.
                if(slope.scale()<0)hold("bad_slope");
                BigInteger mag=power(intercept.mantissa(),p);
                BigInteger scale=intercept.sign()==0 && p.signum()!=0 ?
                    BigInteger.ZERO : p.multiply(BigInteger.valueOf(intercept.scale()));
                int sign=p.signum()==0?1:
                     intercept.sign()==0?0:
                     intercept.sign()<0&&p.testBit(0)?-1:1;
                if(count==Long.MAX_VALUE)hold("line_count_overflow");
                ++count;
                String digits=mag.toString();
                System.out.printf(Locale.ROOT,
                    "SOURCE\t%d\t%s\t%s\t%s\t%d\t%s\t%d\t%s%n",
                    count,cells[0],cells[1],cells[2],sign,scale,digits.length(),digits);
            }
        }
        if(count==0)hold("no_lines");
        System.out.printf(Locale.ROOT,"END\t%d\tRELEASE=HOLD%n",count);
    }
}
