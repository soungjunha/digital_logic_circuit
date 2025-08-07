`timescale 1ns/1ps

module tb_rca16;

    // 입력: reg (자극을 주기 위해)
    reg [15:0] A;
    reg [15:0] B;
    reg Cin;

    // 출력: wire (회로 결과 관찰)
    wire [15:0] Sum;
    wire Cout;

    // 테스트 대상 DUT (Device Under Test)
    rca16 uut (
        .A(A),
        .B(B),
        .Cin(Cin),
        .Sum(Sum),
        .Cout(Cout)
    );

    // 초기 블록: 테스트 벡터 적용
    initial begin
        $display("Time\t\tA\t\tB\t\tCin\t|\tSum\t\tCout");
        $display("---------------------------------------------------------------");

        // 테스트 1: 5 + 3
        A = 16'd5; B = 16'd3; Cin = 1'b0;
        #10 $display("%0t\t%h\t%h\t%b\t|\t%h\t%b", $time, A, B, Cin, Sum, Cout);

        // 테스트 2: 65535 + 1
        A = 16'hFFFF; B = 16'd1; Cin = 1'b0;
        #10 $display("%0t\t%h\t%h\t%b\t|\t%h\t%b", $time, A, B, Cin, Sum, Cout);

        // 테스트 3: 32768 + 32768 (Overflow check)
        A = 16'd32768; B = 16'd32768; Cin = 1'b0;
        #10 $display("%0t\t%h\t%h\t%b\t|\t%h\t%b", $time, A, B, Cin, Sum, Cout);

        // 테스트 4: Random values with Cin
        A = 16'h1234; B = 16'h4321; Cin = 1'b1;
        #10 $display("%0t\t%h\t%h\t%b\t|\t%h\t%b", $time, A, B, Cin, Sum, Cout);

        // 테스트 5: All zeros
        A = 16'h0000; B = 16'h0000; Cin = 1'b0;
        #10 $display("%0t\t%h\t%h\t%b\t|\t%h\t%b", $time, A, B, Cin, Sum, Cout);

        $finish;
    end

endmodule
