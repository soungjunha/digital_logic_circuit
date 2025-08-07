`timescale 1ns/1ps  // 시뮬레이션 시간 단위: 1ns, 정밀도: 1ps

// 40비트 Ripple Carry Adder (rca40) 테스트벤치
module tb_rca40;

    // 입력 신호: reg 타입은 테스트 시 값을 할당할 수 있음
    reg [39:0] A, B;   // 피연산자 A, B (40비트)
    reg Cin;           // 초기 캐리 입력

    // 출력 신호: wire 타입은 회로의 결과를 받아오는 신호
    wire [39:0] S;     // 합 결과 (40비트)
    wire Cout;         // 최종 캐리 출력

    // 테스트 대상 DUT (Device Under Test) 인스턴스화
    rca40 uut (
        .S(S),
        .A(A),
        .B(B),
        .Cout(Cout),
        .Cin(Cin)
    );

    // 테스트 시나리오 정의
    initial begin
        // 콘솔에 결과 출력 헤더 표시
        $display("================ RCA 40-bit Functional Test ================");
        $display("Time\t\tA\t\t\t\tB\t\t\t\tCin | S\t\t\t\t\tCout");
        $display("============================================================");//신호 추적

        // 테스트 벡터 1: 0 + 0 + 0 → 기본 동작 확인 (Sum = 0, Cout = 0)
        A = 40'd0; B = 40'd0; Cin = 0;
        #10 $display("%0t\t%h\t%h\t%b | %h\t%b", $time, A, B, Cin, S, Cout);

        // 테스트 벡터 2: 1 + 1 + 0 → Sum = 2, Cout = 0
        A = 40'd1; B = 40'd1; Cin = 0;
        #10 $display("%0t\t%h\t%h\t%b | %h\t%b", $time, A, B, Cin, S, Cout);

        // 테스트 벡터 3: 최대값(40'hFFFFFFFFFF) + 1 → 오버플로우 확인 (Sum = 0, Cout = 1)
        A = 40'hFFFFFFFFFF; B = 40'd1; Cin = 0;
        #10 $display("%0t\t%h\t%h\t%b | %h\t%b", $time, A, B, Cin, S, Cout);

        // 테스트 벡터 4: 큰 수 + 작은 수 + Cin = 1 → 여러 자리 캐리 전파 확인
        A = 40'h123456789A; B = 40'h000000000F; Cin = 1;
        #10 $display("%0t\t%h\t%h\t%b | %h\t%b", $time, A, B, Cin, S, Cout);

        // 테스트 벡터 5: 모든 비트 1 + 모든 비트 1 + Cin = 1 → Sum = 0xFFFFFFFFFE, Cout = 1
        A = 40'hFFFFFFFFFF; B = 40'hFFFFFFFFFF; Cin = 1;
        #10 $display("%0t\t%h\t%h\t%b | %h\t%b", $time, A, B, Cin, S, Cout);

        // 시뮬레이션 종료
        $finish;
    end

endmodule
