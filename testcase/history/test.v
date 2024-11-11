
module or2(out, in1, in2);
  output out;
  input in1, in2;
  wire in1, in2, out;
  assign out = in1 || in2;
endmodule

module and2(out, in1, in2);
  output out;
  input in1, in2;
  wire in1, in2, out;
  assign out = in1 && in2;
endmodule

module not1(out, in);
  output out;
  input in;
  wire in,out;
  assign out = ~in;
endmodule

module nand2(out, in1, in2);
  output out;
  input in1, in2;
  wire in1, in2, out;
  assign out = ~(in1 && in2);
endmodule

module combLogic( w_0_0, w_0_1, w_0_2, w_0_3, w_5_0, w_5_1, w_5_2 );
  inout w_0_0, w_0_1, w_0_2, w_0_3;
  output w_5_0, w_5_1, w_5_2;
  wire w_0_0, w_0_1, w_0_2, w_0_3;
  wire w_1_0, w_2_0, w_4_0;
  wire w_5_0, w_5_1, w_5_2;
  not1  I1_0(w_1_0, w_0_0);
  or2   I1_2(w_1_1, w_0_3, w_0_1);
  or2   I2_0(w_2_0, w_1_0, w_0_1);
  nand2 I2_1(w_2_1, w_0_2, w_1_1);
  nand2 I4_0(w_4_0, w_2_0, w_0_1);
  and2  I4_1(w_0_3, w_2_1, w_0_2);
  nand2 I5_0(w_5_0, w_4_0, w_0_2);
  nand2 I5_1(w_5_1, w_2_0, w_4_0);
  not1  I5_2(w_5_2, w_0_3);
  not1  I6_0(w_0_0, w_5_0);

  initial begin
    $get_module_info();
  end
endmodule




module tb();
  wire  w_0_0, w_0_1, w_0_2, w_0_3, w_5_0, w_5_1, w_5_2 ;
  reg r0, r1, r2, r3, rEnd; 

  combLogic I0(  w_0_0, w_0_1, w_0_2, w_0_3, w_5_0, w_5_1, w_5_2  );

  assign w_0_1 = r1;
  assign w_0_2 = r2;

  initial begin 
    r0 = 1'b1; 
    r1 = 1'b1; 
    r2 = 1'b1; 
    r3 = 1'b1; 
    $monitor("%t %b %b %b %b %b %b %b  ", $time, w_0_0, w_0_1, w_0_2, w_0_3, w_5_0, w_5_1, w_5_2);
    #10;
    $finish;
  end
  always #1 r0 = ~r0;
  always #2 r1 = ~r1;
  always #4 r2 = ~r2;
  always #8 r3 = ~r3;
endmodule

