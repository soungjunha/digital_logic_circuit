`timescale 1ns/1ps

module tb_rca40_80;

    // 입력 신호 정의
    reg [79:0] A, B;
    reg Cin;

    // 출력 신호 정의
    wire [79:0] Sum;
    wire Cout;

    // DUT (Device Under Test): RCA80 인스턴스화
    rca40_80 uut (
        .A(A),
        .B(B),
        .Cin(Cin),
        .Sum(Sum),
        .Cout(Cout)
    );

    // 시뮬레이션 시작
    initial begin
        // 출력 형식 헤더
        $display("=================================================================================================================");
        $display("Time\t\t\t\tA\t\t\t\t\t\tB\t\t\t\t\t\tCin | Sum\t\t\t\t\t\t\t\t\tCout");
        $display("=================================================================================================================");

        // 테스트 벡터 1: 모두 0
        A = 80'd0; B = 80'd0; Cin = 0;
        #10 $display("%0t\t%h\t%h\t%b | %h\t%b", $time, A, B, Cin, Sum, Cout);

        // 테스트 벡터 2: A만 최대값, B=0, Cin=0
        A = 80'hFFFFFFFFFFFFFFFFFFFF; B = 80'd0; Cin = 0;
        #10 $display("%0t\t%h\t%h\t%b | %h\t%b", $time, A, B, Cin, Sum, Cout);

        // 테스트 벡터 3: B만 최대값, A=0, Cin=0
        A = 80'd0; B = 80'hFFFFFFFFFFFFFFFFFFFF; Cin = 0;
        #10 $display("%0t\t%h\t%h\t%b | %h\t%b", $time, A, B, Cin, Sum, Cout);

        // 테스트 벡터 4: A = 1, B = 1, Cin = 1 (Carry Propagation 확인)
        A = 80'd1; B = 80'd1; Cin = 1;
        #10 $display("%0t\t%h\t%h\t%b | %h\t%b", $time, A, B, Cin, Sum, Cout);

        // 테스트 벡터 5: A = 2^79, B = 2^79, Cin = 0 (MSB overflow 확인)
        A = 80'h80000000000000000000; B = 80'h80000000000000000000; Cin = 0;
        #10 $display("%0t\t%h\t%h\t%b | %h\t%b", $time, A, B, Cin, Sum, Cout);

        // 테스트 벡터 6: A = B = 80비트 중간 값
        A = 80'h7FFFFFFFFFFFFFFFFFFF; B = 80'h7FFFFFFFFFFFFFFFFFFF; Cin = 0;
        #10 $display("%0t\t%h\t%h\t%b | %h\t%b", $time, A, B, Cin, Sum, Cout);

        // 시뮬레이션 종료
        $finish;
    end

endmodule
