// 1비트 Full Adder 모듈 정의
module full_adder (
    input wire A,       // 피가산수 A (1비트)
    input wire B,       // 가산수 B (1비트)
    input wire Cin,     // 이전 자리에서 전달된 캐리 입력 (Carry-in)
    output wire Sum,    // 현재 자리의 합 결과 (1비트)
    output wire Cout    // 다음 자리로 전달할 캐리 출력 (Carry-out)
);
    // 1비트 덧셈: 합은 XOR 세 개로 계산됨
    assign Sum  = A ^ B ^ Cin;
    // 캐리는 두 입력이 모두 1이거나, 하나가 1이고 Carry-in이 1일 때 발생
    assign Cout = (A & B) | (B & Cin) | (A & Cin);
endmodule

// 16비트 Ripple Carry Adder (RCA) 모듈 정의
module rca16 (
    input wire [15:0] A,     // 피가산수 A (16비트)
    input wire [15:0] B,     // 가산수 B (16비트)
    input wire Cin,          // 처음 자리의 캐리 입력
    output wire [15:0] Sum,  // 합 결과 (16비트)
    output wire Cout         // 최종 캐리 출력
);
    // 내부 캐리 신호를 저장할 16비트 와이어 배열
    wire [15:0] carry;

    // 첫 번째 비트(LSB)에 대해 Full Adder 수행
    full_adder FA0 (
        .A    (A[0]),        // A의 0번째 비트
        .B    (B[0]),        // B의 0번째 비트
        .Cin  (Cin),         // 입력 캐리
        .Sum  (Sum[0]),      // 결과의 0번째 비트
        .Cout (carry[0])     // 다음 비트로 전달될 캐리
    );

    // 나머지 15개의 Full Adder를 generate 문을 사용해 반복 생성
    genvar i;
    generate
        for (i = 1; i < 16; i = i + 1) begin : adder_loop
            full_adder FA (
                .A    (A[i]),          // A의 i번째 비트
                .B    (B[i]),          // B의 i번째 비트
                .Cin  (carry[i-1]),    // 이전 비트에서 전달된 캐리 입력
                .Sum  (Sum[i]),        // 결과의 i번째 비트
                .Cout (carry[i])       // 다음 비트로 전달될 캐리 출력
            );
        end
    endgenerate

    // 마지막 자리에서 발생한 캐리를 최종 캐리 출력 Cout에 연결
    assign Cout = carry[15];

endmodule
