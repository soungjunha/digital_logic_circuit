`timescale 1ns/1ps  // 시뮬레이션 시간 단위: 1ns, 정밀도: 1ps

//--------------------------------------
// 1-bit Full Adder (전가산기) 모듈
// - 세 입력 비트(a, b, cin)를 받아 합(sum)과 캐리(cout)를 출력
//--------------------------------------
module full_adder (
    input  wire a,     // 피연산자 A의 단일 비트
    input  wire b,     // 피연산자 B의 단일 비트
    input  wire cin,   // 이전 자리(하위 비트)에서 올라온 캐리 입력
    output wire sum,   // 현재 자리의 합 결과
    output wire cout   // 다음 자리(상위 비트)로 전달할 캐리 출력
);
    // 합: XOR 연산을 통해 세 입력의 홀수 개가 1일 때 1
    assign sum  = a ^ b ^ cin;

    // 캐리: 세 입력 중 두 개 이상이 1일 때 발생 (majority logic)
    assign cout = (a & b) | (a & cin) | (b & cin);
endmodule

//------------------------------------------------------------
// 40-bit Ripple Carry Adder (RCA)
// - 1-bit Full Adder를 40개 직렬로 연결
// - 캐리가 비트 순서대로 ripple(전파)됨
//------------------------------------------------------------
module rca40 (
    output wire [39:0] S,      // 40비트 합 결과 출력
    input  wire [39:0] A, B,   // 40비트 피연산자 A, B
    output wire        Cout,   // 최종(39번째 비트 이후) 캐리 출력
    input  wire        Cin     // 초기 캐리 입력 (일반적으로 0)
);

    // 각 Full Adder 사이를 연결하는 내부 캐리 신호 배열
    wire [39:0] carry;

    // 첫 번째 Full Adder (비트 0, LSB) 인스턴스화
    full_adder FA0 (
        .a    (A[0]),
        .b    (B[0]),
        .cin  (Cin),
        .sum  (S[0]),
        .cout (carry[0])  // 다음 비트의 캐리 입력으로 전달
    );

    // 비트 1~39의 Full Adder들을 반복 생성
    genvar i;
    generate
        for (i = 1; i < 40; i = i + 1) begin : gen_fa
            full_adder FA (
                .a    (A[i]),
                .b    (B[i]),
                .cin  (carry[i-1]),  // 이전 비트에서 전달된 캐리 사용
                .sum  (S[i]),
                .cout (carry[i])     // 다음 비트로 캐리 전파
            );
        end
    endgenerate

    // 마지막 캐리를 최종 캐리 출력에 연결
    assign Cout = carry[39];

endmodule

//------------------------------------------------------------
// 80-bit Ripple Carry Adder (RCA)
// - rca40 모듈 2개를 직렬 연결하여 80비트 덧셈 구현
// - 하위 40비트 RCA의 캐리를 상위 RCA의 입력 캐리로 전달
//------------------------------------------------------------
module rca40_80 (
    output wire [79:0] Sum,     // 최종 합 결과 (80비트)
    input  wire [79:0] A, B,    // 피연산자 A, B (각 80비트)
    output wire        Cout,    // 최상위 비트(79) 이후의 캐리 출력
    input  wire        Cin      // 초기 캐리 입력
);

    // 하위 RCA40의 캐리를 상위 RCA40으로 전달할 연결선
    wire carry_mid;

    // 하위 40비트 덧셈 수행 (bit 0~39)
    rca40 lower40 (
        .S    (Sum[39:0]),     // 합의 하위 40비트
        .A    (A[39:0]),       // A의 하위 40비트
        .B    (B[39:0]),       // B의 하위 40비트
        .Cout (carry_mid),     // 상위 RCA로 전달할 캐리
        .Cin  (Cin)            // 외부에서 전달된 초기 캐리
    );

    // 상위 40비트 덧셈 수행 (bit 40~79)
    rca40 upper40 (
        .S    (Sum[79:40]),    // 합의 상위 40비트
        .A    (A[79:40]),      // A의 상위 40비트
        .B    (B[79:40]),      // B의 상위 40비트
        .Cout (Cout),          // 최종 캐리 출력
        .Cin  (carry_mid)      // 하위 RCA로부터 전달받은 캐리 입력
    );

endmodule
