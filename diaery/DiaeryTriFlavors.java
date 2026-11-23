// SPDX-License-Identifier: 0BSD
// DIAERY tri-flavors | Oracle JDK 27.0.1.1 | standalone Java profile.
// The Oracle JDK is a toolchain dependency and is not redistributed.
// This independent verifier DOES NOT dereference the XHTML's external DTD.
// No application network requests or scripts. This is exact fixture
// qualification, not a complete XHTML 1.0 Strict parser.
import java.math.BigInteger;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

public final class DiaeryTriFlavors {
    private static final String VERSION = "27.0.1.1";
    private static final String[] PAGE = {
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>",
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">",
        "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">",
        "<head><title>DIAERY | Tri-Flavor Concurrence</title></head>",
        "<body>",
        "<h1>DIAERY Tri-Flavor Concurrence</h1>",
        "<p>TRIFLAVOR-1|oracle=27.0.1.1|jig_x1e17=2864498081537971875</p>",
        "<p>TRIFLAVOR-NICE|triangle=30,40|radius2_min=2500|below=0|at=1|above=1|sweep=4096</p>",
        "<p>TRIFLAVOR-LOOP|steps=64|jig_closed=0|e_closed=0|jig_gap_x1e18=278772184302000000|e_gap_x1e18=29962978621104960|normalized_closed=1</p>",
        "<p>TRIFLAVOR-WATCHDOG|monotone=1|fail_closed=1|release=HOLD</p>",
        "</body>",
        "</html>",
        ""
    };
    private static final BigInteger JIG = new BigInteger("2864498081537971875");
    private static final BigInteger E_REFERENCE = new BigInteger("2718281828459045235");
    private static final BigInteger JSCALE = BigInteger.TEN.pow(17);
    private static final BigInteger ESCALE = BigInteger.TEN.pow(18);

    private static boolean nice(long radiusSquared) {
        if (radiusSquared == 0L) return false;
        final int[][] vertices = {{0, 0}, {30, 0}, {0, 40}};
        final int twiceArea = 30 * 40;
        if (twiceArea == 0) return false;
        for (int[] v : vertices) {
            long dx = 2L * v[0] - 30L, dy = 2L * v[1] - 40L;
            if (dx * dx + dy * dy > radiusSquared) return false;
        }
        return true;
    }
    private static boolean sweep() {
        boolean seen = false;
        int first = 0;
        for (int radius2 = 1; radius2 <= 4096; ++radius2) {
            boolean pass = nice(radius2);
            if (pass && !seen) { seen = true; first = radius2; }
            if (seen && !pass) return false;
        }
        return first == 2500 && !nice(2499) && nice(2500) && nice(2501);
    }
    private static BigInteger gap(BigInteger fraction, BigInteger scale) {
        BigInteger r = fraction.multiply(BigInteger.valueOf(64)).mod(scale);
        return r.min(scale.subtract(r));
    }
    private static void check(boolean condition, String why) {
        if (!condition) throw new IllegalStateException("TRIFLAVOR|HOLD|" + why);
    }
    public static void main(String[] args) throws Exception {
        check(args.length == 1, "ARGUMENTS");
        check(System.getProperty("java.vendor").equals("Oracle Corporation")
                && System.getProperty("java.version").equals(VERSION),
                "WRONG_ORACLE_JDK");
        byte[] bytes = Files.readAllBytes(Path.of(args[0]));
        check(bytes.length > 0 && bytes.length <= 16000, "DOCUMENT_SIZE");
        for (byte b : bytes) {
            int v = b & 255;
            check(v == 10 || (v >= 32 && v <= 126), "NON_ASCII_PROFILE");
        }
        String doc = new String(bytes, StandardCharsets.US_ASCII);
        check(doc.equals(String.join("\n", PAGE)), "XHTML_STRICT_FIXTURE");
        check(JIG.equals(new BigInteger("1833278772184302")
              .multiply(BigInteger.valueOf(100000))
              .divide(BigInteger.valueOf(64))), "JIG_SOURCE");
        check(sweep(), "NON_MONOTONE_NICE");
        BigInteger j = gap(JIG.remainder(JSCALE), JSCALE)
                       .multiply(BigInteger.TEN);
        BigInteger e = gap(E_REFERENCE.remainder(ESCALE), ESCALE);
        check(j.equals(new BigInteger("278772184302000000"))
                && e.equals(new BigInteger("29962978621104960")),
                "EXACT_LOOP_GAPS");
        check(j.signum() > 0 && e.signum() > 0, "UNJUSTIFIED_CLOSURE");
        check(ESCALE.remainder(BigInteger.valueOf(64)).signum() == 0,
              "NORMALIZED_CLOSURE");
        System.out.println("TRIFLAVOR|PASS|JIG_X1E17=2864498081537971875|RADIUS2_MIN=2500|JIG_GAP_X1E18=278772184302000000|REFERENCE_GAP_X1E18=29962978621104960|SWEEP=4096|NORMALIZED_CLOSED=1|RELEASE=HOLD");
    }
}
