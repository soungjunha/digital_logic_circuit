module full_adder (
    input wire A,
    input wire B,
    input wire Cin,
    output wire Sum,
    output wire Cout
);
    assign Sum  = A ^ B ^ Cin;
    assign Cout = (A & B) | (B & Cin) | (A & Cin);
endmodule
//==============================
// 16비트 Ripple Carry Adder 모듈 정의
// - 1비트 full_adder를 16개 직렬 연결
//==============================
module RCA16 (
    input wire [15:0] A,     // 16비트 피연산자 A
    input wire [15:0] B,     // 16비트 피연산자 B
    input wire Cin,          // 초기 캐리 입력 (보통 0 또는 상위 모듈에서 전달됨)
    output wire [15:0] Sum,  // 16비트 덧셈 결과
    output wire Cout         // 최종 캐리 출력 (다음 상위 모듈에 전달)
);
    // 각 full_adder 간의 내부 캐리 연결을 위한 와이어 배열
    wire [15:0] carry;

    // 첫 번째 full_adder (LSB) 인스턴스
    full_adder FA0 (
        .A    (A[0]),
        .B    (B[0]),
        .Cin  (Cin),
        .Sum  (Sum[0]),
        .Cout (carry[0])
    );

    // 나머지 15개의 full_adder를 generate 문으로 자동 생성
    genvar i;
    generate
        for (i = 1; i < 16; i = i + 1) begin : adder_loop
            full_adder FA (
                .A    (A[i]),
                .B    (B[i]),
                .Cin  (carry[i-1]),  // 이전 자리의 캐리 입력
                .Sum  (Sum[i]),
                .Cout (carry[i])     // 다음 자리로 전달할 캐리 출력
            );
        end
    endgenerate

    // 최종 캐리 출력: 마지막 자리에서 생성된 캐리를 Cout으로 연결
    assign Cout = carry[15];
endmodule

//==============================
// 80비트 Ripple Carry Adder 모듈 정의
// - RCA16 모듈 5개를 직렬로 연결하여 80비트 덧셈 수행
//==============================
module rca16_80 (
    input wire [79:0] A,      // 80비트 피연산자 A
    input wire [79:0] B,      // 80비트 피연산자 B
    input wire Cin,           // 초기 캐리 입력
    output wire [79:0] Sum,   // 80비트 합 결과
    output wire Cout          // 최종 캐리 출력
);
    // 각 RCA16 블록 간 캐리 전달을 위한 내부 와이어
    wire [4:0] carry;

    // RCA16 블록 1 (하위 16비트: 0~15)
    RCA16 rca0 (
        .A    (A[15:0]),
        .B    (B[15:0]),
        .Cin  (Cin),          // 외부 입력 캐리
        .Sum  (Sum[15:0]),
        .Cout (carry[0])      // 다음 RCA16 블록으로 전달될 캐리
    );

    // RCA16 블록 2 (비트 16~31)
    RCA16 rca1 (
        .A    (A[31:16]),
        .B    (B[31:16]),
        .Cin  (carry[0]),
        .Sum  (Sum[31:16]),
        .Cout (carry[1])
    );

    // RCA16 블록 3 (비트 32~47)
    RCA16 rca2 (
        .A    (A[47:32]),
        .B    (B[47:32]),
        .Cin  (carry[1]),
        .Sum  (Sum[47:32]),
        .Cout (carry[2])
    );

    // RCA16 블록 4 (비트 48~63)
    RCA16 rca3 (
        .A    (A[63:48]),
        .B    (B[63:48]),
        .Cin  (carry[2]),
        .Sum  (Sum[63:48]),
        .Cout (carry[3])
    );

    // RCA16 블록 5 (최상위 비트 64~79)
    RCA16 rca4 (
        .A    (A[79:64]),
        .B    (B[79:64]),
        .Cin  (carry[3]),
        .Sum  (Sum[79:64]),
        .Cout (Cout)           // 최종 캐리 출력
    );
endmodule
