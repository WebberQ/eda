// Gate Level Verilog Code Generated!
// GateLvl:20 GateNum:20 GateInputNum:2
// ****** Basic Gate Module Defination ******
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
// ****** Basic Gate Module Defination End ******

// ****** Combined Logic Module Defination ******
module combLogic(w_003_001, w_003_002, w_003_003, w_003_004, w_003_005, w_003_006, w_003_007, w_003_008);

  input w_003_001, w_003_002, w_003_003, w_003_004, w_003_005, w_003_006, w_003_007, w_003_008;
  wire  w_003_001, w_003_002, w_003_003, w_003_004, w_003_005, w_003_006, w_003_007, w_003_008;

  wire w_000_001, w_000_002, w_000_003, w_000_004, w_000_005;
  wire w_001_006, w_001_007, w_001_008, w_001_009, w_001_010;
  wire w_002_012, w_002_013, w_002_014, w_002_015;

  wire w_add1_001, w_add1_002, w_add1_003;
  wire OscFlag,OscFlag1,OscFlag2,OscFlag3,OscFlag12;
  wire w_add2_001,w_add2_002,w_add2_003,w_add2_004,w_add2_005,w_add2_007,w_add2_008;
  wire w_add3_001,w_add3_002,w_add3_003,w_add3_004,w_add3_005;

  not1    I001_001(w_000_001, w_000_002);
  and2    I001_002(w_000_002, w_003_001, w_000_003);
  and2    I001_003(w_000_004, w_003_002, w_000_005);
  and2    I001_004(w_001_006, w_000_005, w_003_003);
  //or2     I001_005(w_001_007, w_003_004, w_000_003);
  and2     I001_005(w_001_007, w_003_004, w_000_003);
  and2    I001_006(w_001_008, w_000_001, w_000_004);

  //nand2   I001_007(w_000_005, w_001_009, w_001_010);环三原电路改造
  nand2   I001_007(w_000_005, w_001_009, w_change3);
  or2     Ichange3(w_chang3,OscFlag3,w_001_010);

  //or2     I001_008(w_002_012, w_001_006, w_001_007);
  and2    I001_008(w_002_012, w_001_006, w_001_007);

  //nand2   I001_009(w_000_003, w_002_013, w_002_014);环一环二原电路改造
  nand2   I001_009(w_000_003, w_chang2, w_change1);
  or2     Ichange1(w_change1,OscFlag1,w_002_014);
  or2     Ichange2(w_change2,OscFlag2,w_002_013);   

  nand2   I001_010(w_001_009, w_003_005, w_001_008);
  //or2     I002_012(w_001_010, w_002_012, w_003_006);
  and2     I002_012(w_001_010, w_002_012, w_003_006);
  and2    I002_013(w_002_013, w_003_007, w_002_012);
  not1    I002_014(w_002_014, w_002_015);
  and2    I002_015(w_002_015, w_001_008, w_003_008);
  
  //每个环内部条件
  and2    Iadd1_001(w_add1_001,w_003_001,w_003_008);
  and2    Iadd2_001(w_add2_001,w_003_004,w_003_007);
  and2    Iadd3_001(w_add3_001,w_003_003,w_003_006);
  
  //环一osc条件
  and2    Iadd1_002(w_add1_002,w_003_003,w_003_002);
  and2    Iadd1_003(w_add1_003,w_add1_001,w_add1_002);
  and2    I_oscflag1(OscFlag1,w_add1_003,w_add2_001);

  //环二osc条件
  not1    I_w2(wn_003_002,w_003_002);
  not1    I_w8(wn_003_008,w_003_008);
  or2     I_add2_002(w_add2_002,wn_003_002,wn_003_008);
  not1    I_w6(wn_003_006,w_003_006);
  and2    I_add2_003(w_add2_003,wn_003_006,w_add2_002);
  not1    I_w5(wn_003_005,w_003_005);
  or2     I_add2_004(w_add2_004,wn_003_005,w_003_001);
  and2    I_add2_005(w_add2_005,w_add2_004,w_003_006);
  or2     I_add2_007(w_add2_007,w_add2_003,w_add2_005);
  and2    I_add2_008(w_add2_008,w_003_003,w_add2_007);
  and2    I_oscflag2(OscFlag2,w_add2_001,w_add2_008);

  //环三osc条件
  not1    I_w7(wn_003_007,w_003_007);
  and2    I_add3_002(w_add3_002,wn_003_007,wn_003_002);
  or2     I_add3_003(w_add3_003,wn_003_005,w_003_001);
  or2     I_add3_004(w_add3_004,w_add3_003,w_add3_002);
  and2    I_add3_005(w_add3_005,w_003_004,w_add3_004);
  and2    I_oscflag3(OscFlag3,w_add3_001,w_add3_005);
  
  //Osc条件
  or2    I_oscflag12(OscFlag12,OscFlag1,OscFlag2);
  or2    I_oscflag(OscFlag,OscFlag12,OscFlag3);


  





  initial begin
    //$get_module_info();
  end
endmodule

// ****** Combined Logic Module Defination ******

// ****** TestBench Module Defination ******

module tb();
  wire  w_003_001, w_003_002, w_003_003, w_003_004, w_003_005, w_003_006, w_003_007, w_003_008;
  combLogic I0( w_003_001, w_003_002, w_003_003, w_003_004, w_003_005, w_003_006, w_003_007, w_003_008 );

  reg r1, r2, r3, r4, r5, r6, r7, r8; 

  assign w_003_001 = r1;
  assign w_003_002 = r2;
  assign w_003_003 = r3;
  assign w_003_004 = r4;
  assign w_003_005 = r5;
  assign w_003_006 = r6;
  assign w_003_007 = r7;
  assign w_003_008 = r8;

  initial begin 
    $deposit(I0.w_002_013, 1'b1);
    r1 = 1'b0; 
    r2 = 1'b0; 
    r3 = 1'b0; 
    r4 = 1'b0; 
    r5 = 1'b0; 
    r6 = 1'b0; 
    r7 = 1'b0; 
    r8 = 1'b0; 
    $monitor("  Time=%-3t, Signals: [%b %b %b %b %b %b %b %b]  [%b %b %b %b %b %b %b %b %b %b %b %b %b %b] ", $time, 
                 w_003_001, w_003_002, w_003_003, w_003_004, w_003_005, w_003_006, w_003_007, w_003_008, 
                 I0.w_000_001, I0.w_000_002, I0.w_000_003, I0.w_000_004, I0.w_000_005,
                 I0.w_001_006, I0.w_001_007, I0.w_001_008, I0.w_001_009, I0.w_001_010,
                 I0.w_002_012, I0.w_002_013, I0.w_002_014, I0.w_002_015  );
    #260;
    $finish;
  end

  always #1   r1 = ~r1;
  always #2   r2 = ~r2;
  always #4   r3 = ~r3;
  always #8   r4 = ~r4;
  always #16  r5 = ~r5;
  always #32  r6 = ~r6;
  always #64  r7 = ~r7;
  always #128 r8 = ~r8;


endmodule


