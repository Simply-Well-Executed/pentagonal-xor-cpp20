/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// No JUnit, Gradle, Maven, Android runtime or third-party testing dependency.
package com.simplywellexecuted.fastestwhisper10d;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public final class StandaloneTest {
    private StandaloneTest() {}
    private static int checks = 0;

    private static void require(boolean test, String message) {
        if (!test) throw new AssertionError(message);
        checks++;
    }
    private static void near(double value, double expected, String label) {
        require(Double.isFinite(value) &&
                Math.abs(value-expected) <= 1e-12 * Math.max(1,Math.abs(expected)),
                label + ": " + value + " expected " + expected);
    }
    private static double[] nine(double value) {
        double[] x = new double[9];
        java.util.Arrays.fill(x, value);
        return x;
    }
    private static double independentParity(double[] h, int ring) {
        double sum=0.0;
        for (int mask=0; mask<32; mask++) {
            if ((Integer.bitCount(mask)&1)==0) continue;
            double term=1.0;
            for(int j=0;j<5;j++) {
                double v=h[Stability.RINGS[ring][j]];
                term *= (mask&(1<<j))!=0 ? v : (1.0-v);
            }
            sum += term;
        }
        return sum;
    }
    private static void goldens() {
        Stability.Result zero=Stability.calculate(nine(0.0));
        require("000000000000".equals(zero.hexabyte) && zero.scalar==1.0,
                "zero vector");
        Stability.Result ones=Stability.calculate(nine(1.0));
        require("ffffffffffff".equals(ones.hexabyte) && ones.scalar==1.0,
                "one vector");
        Stability.Result halves=Stability.calculate(nine(0.5));
        require("008000800080".equals(halves.hexabyte), "half quantization");
        double[] overlap=nine(0.0);
        overlap[3]=1.0;
        Stability.Result crossed=Stability.calculate(overlap);
        require("ffffffff0000".equals(crossed.hexabyte) &&
                crossed.scalar==0.0, "interlocked parity");
    }
    private static void minterms() {
        int state=0x12578A3D;
        for (int trial=0;trial<2048;trial++) {
            double[] values=new double[9];
            for(int i=0;i<9;i++) {
                state=state*1664525+1013904223;
                values[i]=((state>>>16)%1001)/1000.0;
            }
            double[] original=values.clone();
            Stability.Result result=Stability.calculate(values);
            require(java.util.Arrays.equals(original,values),"source changed");
            for(int ring=0;ring<3;ring++) {
                double expected=independentParity(values,ring);
                near(result.probabilities[ring],expected,"minterm ring");
                // Verify exact serialization of the ACTUAL production
                // score. Independent minterms have different arithmetic
                // order and may round 1 U16 apart only at a half-step tie.
                final int word=(int)Math.floor(
                        result.probabilities[ring]*65535.0+0.5);
                require(result.words[ring]==word,"U16 production mismatch");
                final int referenceWord=(int)Math.floor(expected*65535.0+0.5);
                if (word!=referenceWord) {
                    final double threshold=Math.min(word,referenceWord)+0.5;
                    require(Math.abs(word-referenceWord)==1 &&
                            Math.abs(expected*65535.0-threshold)<1e-9,
                            "quantization mismatch outside the half-step boundary");
                }
                require((result.hexabyteBytes[2*ring]&255)==(word&255),
                        "low byte mismatch");
                require((result.hexabyteBytes[2*ring+1]&255)==((word>>>8)&255),
                        "high byte mismatch");
            }
        }
    }
    private static void normalization() {
        double[] source={3.0,2.0,20.0,Math.sqrt(14.0/3.0),
                Math.sqrt(1400.0/3.0),3.0,30.0,0.0,0.0};
        double[] out=Stability.normalize(source);
        near(out[0],3.0/1027.0,"frame count");
        near(out[1],0.5*(1.0+2.0/3.0),"left mean");
        near(out[7],0.0,"crossings");
    }
    private static void rejects() {
        boolean caught=false;
        try { Stability.calculate(new double[8]); }
        catch (IllegalArgumentException e) { caught=true; }
        require(caught,"wrong ligand count accepted");

        double[] bad=nine(0.0);
        bad[7]=Double.NaN;
        caught=false;
        try { Stability.calculate(bad); }
        catch (IllegalArgumentException e) { caught=true; }
        require(caught,"NaN accepted");

        bad=nine(0.0);
        bad[2]=1.1;
        caught=false;
        try { Stability.calculate(bad); }
        catch (IllegalArgumentException e) { caught=true; }
        require(caught,"out of bounds accepted");
    }
    private static byte[] frames(float... samples) {
        ByteBuffer buffer=ByteBuffer.allocate(samples.length*4)
                .order(ByteOrder.LITTLE_ENDIAN);
        for(float value:samples) buffer.putFloat(value);
        return buffer.array();
    }
    private static void waveform() throws IOException {
        byte[] original=frames(1,10,2,20,3,30);
        Acoustic10D.Analysis out=Acoustic10D.inspect(new ByteArrayInputStream(original));
        require(out.frames==3 && out.vector.length==10, "vector shape");
        near(out.vector[1],2.0,"mean L");
        near(out.vector[2],20.0,"mean R");
        near(out.legacyPearson,1.0,"correlation");
        near(out.vector[9],out.stability.scalar,"Stability index");
        require(out.stability.hexabyte.length()==12, "hexabyte length");
        require(out.inputSha256.length()==64, "SHA256 length");
    }
    private static void malformed() {
        byte[][] inputs={new byte[0],new byte[7],frames(Float.NaN,0f)};
        for(byte[] bytes:inputs) {
            boolean threw=false;
            try { Acoustic10D.inspect(new ByteArrayInputStream(bytes)); }
            catch (IOException e) { threw=true; }
            require(threw,"invalid source accepted");
        }
    }
    public static void main(String[] args) throws IOException {
        goldens();
        minterms();
        normalization();
        rejects();
        waveform();
        malformed();
        System.out.println("PASS: "+checks+" independent Java checks (Stability + raw audio). "+
                "No Gradle/JUnit/model weights.");
    }
}
