// SPDX-License-Identifier: 0BSD
// Independent Oracle Java 27.0.1.1 model: logical widths, BigInteger, bounded MXRING.
import java.math.BigInteger;
import java.util.ArrayDeque;
public final class SlipSlidingToolongOracle {
 private static final int[] W={32,64,128,192,224,256,384,448,480,512};
 private static final BigInteger ONE=BigInteger.ONE;
 private static void check(boolean condition,String reason){
  if(!condition)throw new IllegalStateException("SLIPSLIDINGTOOLONG|HOLD|"+reason);
 }
 private static boolean fits(BigInteger v,int bits){return v.signum()>=0&&v.bitLength()<=bits;}
 private record Guarded(BigInteger value,int bits,int tag,BigInteger witness) {
  boolean valid(){return fits(value,bits)&&witness.equals(value.xor(BigInteger.valueOf(tag)));}
 }
 private static Guarded owned(BigInteger value,int bits,int tag) {
  check(fits(value,bits),"owned_bound");return new Guarded(value,bits,tag,value.xor(BigInteger.valueOf(tag)));
 }
 public static void main(String[] args){
  check(args.length==0,"unexpected_args");
  check("Oracle Corporation".equals(System.getProperty("java.vendor")),"oracle_vendor");
  check("27.0.1.1".equals(System.getProperty("java.version")),"oracle_version");
  BigInteger low=new BigInteger("A1B2C3D4",16);
  int pairs=0,down=0,up=0,losses=0;
  for(int i=0;i<W.length;i++){
   Guarded src=owned(low,W[i],i+1);
   check(src.valid(),"source_guard");
   for(int j=0;j<W.length;j++){
    check(fits(src.value(),W[j]),"unexpected_loss");
    Guarded moved=owned(src.value(),W[j],src.tag());
    Guarded back=owned(moved.value(),W[i],src.tag());
    check(moved.valid()&&back.valid()&&back.value().equals(src.value()),"roundtrip");
    pairs++;
    if(W[j]<W[i])down++;
    if(W[j]>W[i])up++;
   }
   if(i>0){
    BigInteger high=ONE.shiftLeft(W[i]-1).or(BigInteger.valueOf(0x1234));
    for(int j=0;j<i;j++){
     check(!fits(high,W[j]),"silent_narrowing");
     losses++;
    }
   }
  }
  check(pairs==100&&down==45&&up==45&&losses==45,"count");
  BigInteger mask=ONE.shiftLeft(32).subtract(ONE);
  check(mask.add(ONE).bitLength()==33,"carry");
  check(BigInteger.ZERO.subtract(ONE).and(mask).equals(mask),"borrow");
  check(new BigInteger("f0f0f0f0",16).and(new BigInteger("00ff00ff",16))
   .equals(new BigInteger("00f000f0",16)),"and_mask");
  BigInteger sample=new BigInteger("80000001",16);
  check(sample.shiftLeft(1).shiftRight(1).equals(sample),"shift_roundtrip");
  check(ONE.shiftLeft(511).shiftLeft(1).bitLength()>512,"shift_discard");
  BigInteger result=new BigInteger("315868046760674477")
   .multiply(new BigInteger("100000000000000000")).shiftRight(62);
  check(result.equals(new BigInteger("6849296450333523")),"q62_output");
  check(result.bitLength()<=64&&result.bitLength()>32,"q62_loss_32");
  ArrayDeque<Guarded> ring=new ArrayDeque<>(8);
  Guarded a=owned(ONE,64,71),b=owned(BigInteger.TWO,512,72);
  ring.addLast(a);ring.addLast(b);
  check(ring.size()==2&&ring.peekFirst().valid(),"ring_observation");
  check(ring.removeFirst().equals(a)&&ring.removeFirst().equals(b),"ring_fifo");
  for(int i=0;i<8;i++)ring.addLast(a);
  check(ring.size()==8,"ring_count");
  check(!new Guarded(a.value(),a.bits(),a.tag(),BigInteger.ZERO).valid(),"guard_corruption");
  System.out.println("SLIPSLIDINGTOOLONG|WIDTHS=10|PAIRS=100|GLINTING=45|GLIZZING=45|EQUAL=10|PASS");
  System.out.println("SLIPSLIDINGTOOLONG|NARROWING_LOSS_REJECTED=45|Q62_128_TO_64=PASS|32BIT_LOSS_REJECTED=PASS");
  System.out.println("GUARDMEMORYGUARD|JAVA_BIGINT_MODEL=PASS|MXRING_FIFO=PASS|SHIFTS=PASS|AND=PASS");
  System.out.println("GUARDMEMORYGUARD|THREAD_SAFE=NOT_PROVEN|CRASH_DURABLE=NOT_PROVEN|RELEASE=HOLD");
 }
}
