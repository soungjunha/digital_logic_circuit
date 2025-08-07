`timescale 1ns/1ps  // 시뮬레이션 시간 단위: 1ns, 시간 정밀도: 1ps

//--------------------------------------
// 1-bit Full Adder (전가산기) 모듈 정의
//--------------------------------------
module full_adder (
    input  wire a,     // 피연산자 A의 단일 비트 입력
    input  wire b,     // 피연산자 B의 단일 비트 입력
    input  wire cin,   // 이전 자리에서 전달된 캐리 입력 (Carry-in)
    output wire sum,   // 현재 자리의 합 출력
    output wire cout   // 다음 자리로 전달될 캐리 출력 (Carry-out)
);
    // 1비트 덧셈 로직: 합은 XOR 세 입력
    assign sum  = a ^ b ^ cin;

    // 캐리 발생 조건: 세 입력 중 두 개 이상이 1일 때 캐리 발생
    assign cout = (a & b) | (a & cin) | (b & cin);
endmodule

//------------------------------------------------------------
// 40-bit Ripple Carry Adder (RCA) 모듈 정의
// - 1비트 Full Adder 40개 직렬 연결
// - 포트 순서: S (Sum), A, B, Cout, Cin
//------------------------------------------------------------
module rca40 (
    output wire [39:0] S,      // 최종 합 결과 (40비트)
    input  wire [39:0] A, B,   // 피연산자 A, B (각각 40비트)
    output wire        Cout,   // 최상위 비트에서 발생한 최종 캐리 출력
    input  wire        Cin     // 가장 하위 비트에 입력되는 초기 캐리
);

    // 내부 캐리 전달을 위한 40비트 와이어 배열
    wire [39:0] carry;

    // 첫 번째 Full Adder (LSB, bit 0) 인스턴스
    full_adder FA0 (
        .a    (A[0]),     // A의 0번째 비트
        .b    (B[0]),     // B의 0번째 비트
        .cin  (Cin),      // 외부 입력 캐리
        .sum  (S[0]),     // 합의 0번째 비트
        .cout (carry[0])  // 다음 자리로 전달할 캐리
    );

    // 1번째 비트부터 39번째 비트까지 나머지 Full Adder 생성
    genvar i;
    generate
        for (i = 1; i < 40; i = i + 1) begin : gen_fa
            full_adder FA (
                .a    (A[i]),         // A의 i번째 비트
                .b    (B[i]),         // B의 i번째 비트
                .cin  (carry[i-1]),   // 이전 자리의 캐리 입력
                .sum  (S[i]),         // 합의 i번째 비트
                .cout (carry[i])      // 다음 자리로 전달될 캐리
            );
        end
    endgenerate

    // 최종 캐리 출력: 마지막 자리(39번째 비트)에서 발생한 캐리
    assign Cout = carry[39];

endmodule
