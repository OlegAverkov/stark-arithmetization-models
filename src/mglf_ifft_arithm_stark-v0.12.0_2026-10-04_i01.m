//mglf_ifft_arithm_stark-v0.12.0_2026-10-04_i01.m 

ProgramStartTime := Realtime();
RealNumbers := RealField();
TotalProgramTimes := [ RealNumbers | ];

NumberOfRuns := 1;

assert NumberOfRuns gt 0;



function IsPowerOfTwoInteger(N)
    if N lt 1 then
        return false;
    end if;

    M := N;

    while M gt 1 do
        if M mod 2 ne 0 then
            return false;
        end if;

        M div:= 2;
    end while;

    return true;
end function;


function NthRootOfUnity(F, N)
    error if not IsPowerOfTwoInteger(N),
        "N must be a positive power of two";

    error if (#F - 1) mod N ne 0,
        "N must divide #F - 1";

    G := PrimitiveElement(F);
   
    W := G^((#F - 1) div N);

    assert W^N eq F!1;

    if N gt 1 then
        assert W^(N div 2) ne F!1;
    end if;

    return W;
end function;


function FFTRecursive(X, W)
    N := #X;

    if N eq 1 then
        return X;
    end if;

    F := Universe(X);
    HalfN := N div 2;

    
    Even := [ F | X[2*i + 1] : i in [0..HalfN - 1] ];

   
    Odd := [ F | X[2*i + 2] : i in [0..HalfN - 1] ];

    EvenFFT := FFTRecursive(Even, W^2);
    OddFFT  := FFTRecursive(Odd,  W^2);

    Y := [ F | 0 : i in [1..N] ];
    Wk := F!1;

    for k := 0 to HalfN - 1 do
        T := Wk * OddFFT[k + 1];

        Y[k + 1] :=
            EvenFFT[k + 1] + T;

        Y[k + HalfN + 1] :=
            EvenFFT[k + 1] - T;

        Wk *:= W;
    end for;

    return Y;
end function;


procedure FFTWithRoot(~X, W)
    N := #X;

    error if not IsPowerOfTwoInteger(N),
        "The sequence length must be a positive power of two";

    F := Universe(X);

    error if Parent(W) ne F,
        "W and X must belong to the same field";

    error if W^N ne F!1,
        "W is not an N-th root of unity";

    if N gt 1 then
        error if W^(N div 2) eq F!1,
            "W is not a primitive N-th root of unity";
    end if;

    X := FFTRecursive(X, W);
end procedure;


procedure FFT(~X)
    N := #X;

    error if not IsPowerOfTwoInteger(N),
        "The sequence length must be a positive power of two";

    F := Universe(X);
    W := NthRootOfUnity(F, N);

    FFTWithRoot(~X, W);
end procedure;


procedure IFFTWithRoot(~X, W)
    N := #X;

    error if not IsPowerOfTwoInteger(N),
        "The sequence length must be a positive power of two";

    F := Universe(X);

    error if Parent(W) ne F,
        "W and X must belong to the same field";

    error if W^N ne F!1,
        "W is not an N-th root of unity";

    if N gt 1 then
        error if W^(N div 2) eq F!1,
            "W is not a primitive N-th root of unity";
    end if;

   
    X := FFTRecursive(X, W^-1);

   
    NInverse := (F!N)^-1;

    X := [
        F |
        NInverse * Value : Value in X
    ];
end procedure;


procedure IFFT(~X)
    N := #X;

    error if not IsPowerOfTwoInteger(N),
        "The sequence length must be a positive power of two";

    F := Universe(X);
    W := NthRootOfUnity(F, N);

    IFFTWithRoot(~X, W);
end procedure;


RealNumbers := RealField();
RunTimes := [ RealNumbers | ];


InterpolationTimes := [ RealNumbers | ];
ProgramWithoutInterpolationTimes := [ RealNumbers | ];



OutputFile := Open("run_times.csv", "w");


fprintf OutputFile, "run,seconds\n";
Flush(OutputFile);


PerRunSectionOutputFileName :=
    "interpolation_and_remaining_program_times_by_run.csv";

PerRunSectionOutputFile :=
    Open(PerRunSectionOutputFileName, "w");

fprintf PerRunSectionOutputFile,
    "run,interpolation_seconds,program_without_interpolation_seconds\n";

Flush(PerRunSectionOutputFile);

function FibonacciSequenceInField(F, Count)

    error if Count lt 1,
        "The number of Fibonacci elements must be positive";

    FibonacciValues := [ F | ];

    PreviousValue := F!1;
    CurrentValue  := F!1;

    for i := 1 to Count do
        Append(~FibonacciValues, PreviousValue);

        NextValue := PreviousValue + CurrentValue;
        PreviousValue := CurrentValue;
        CurrentValue := NextValue;
    end for;

    assert #FibonacciValues eq Count;

    return FibonacciValues;
end function;

function FibonacciIntegerSequence(Count)

    error if Count lt 1,
        "The number of Fibonacci elements must be positive";

    Z := Integers();

    FibonacciValues := [ Z | ];

    PreviousValue := Z!1;
    CurrentValue  := Z!1;

    for i := 1 to Count do
        Append(~FibonacciValues, PreviousValue);

        NextValue := PreviousValue + CurrentValue;
        PreviousValue := CurrentValue;
        CurrentValue := NextValue;
    end for;

    assert #FibonacciValues eq Count;

    return FibonacciValues;
end function;

function ManualPrimitiveRoot(F, RootValue, N)
    error if not IsPowerOfTwoInteger(N),
        "N must be a positive power of two";

    error if (#F - 1) mod N ne 0,
        "N must divide #F - 1";

    W := F!RootValue;

    error if W eq F!0,
        "Zero cannot be a root of unity";

    error if Order(W) ne N,
        "The manually selected element does not have exact order N";

    assert W^N eq F!1;

    if N gt 1 then
        assert W^(N div 2) ne F!1;
    end if;

    return W;
end function;

function ManualRootFromPrimitiveElement(F, GeneratorValue, N)
    error if not IsPowerOfTwoInteger(N),
        "N must be a positive power of two";

    error if (#F - 1) mod N ne 0,
        "N must divide #F - 1";

    G := F!GeneratorValue;

    error if G eq F!0,
        "Zero cannot generate the multiplicative group";

    error if Order(G) ne #F - 1,
        "The manually selected element is not primitive in F";

    W := G^((#F - 1) div N);

    error if Order(W) ne N,
        "The derived root does not have exact order N";

    assert W^N eq F!1;

    if N gt 1 then
        assert W^(N div 2) ne F!1;
    end if;

    return W;
end function;




print "================================================";



p := 2^64 - 2^32 + 1;

assert IsPrime(p);

F := GF(p);
printf "=== FFT/IFFT over GF(%o) ===\n", p;


 
SupportedRootOrder := 2^16;

ManualRootValue := 6115771955107415310;//root = 6115771955107415310 for 2^16
//ManualRootValue := 12380578893860276750;//root = 12380578893860276750 for 2^17
//ManualRootValue := 9306717745644682924;//root = 9306717745644682924 for 2^18
//ManualRootValue := 18146160046829613826;//root = 18146160046829613826 for 2^19
//ManualRootValue := 3511170319078647661;//root = 3511170319078647661 for 2^20
//ManualRootValue := 17654865857378133588;//root = 17654865857378133588 for 2^21
//ManualRootValue := 5416168637041100469;//root = 5416168637041100469 for 2^22
//ManualRootValue := 16905767614792059275;//root = 16905767614792059275 for 2^23
//ManualRootValue := 9713644485405565297;//root = 9713644485405565297 for 2^24
//ManualRootValue := 9306717745644682924;//root = 9306717745644682924 for 2^18
//ManualRootValue := 5456943929260765144;//root = 5456943929260765144 for 2^25
//ManualRootValue := 17096174751763063430;//root = 17096174751763063430 for 2^26 
W30 := ManualPrimitiveRoot(
    F,
    ManualRootValue,
    SupportedRootOrder
);


assert W30^SupportedRootOrder eq F!1;
assert W30^(SupportedRootOrder div 2) ne F!1;

print "Primitive root of order 2^16:";
print W30;
 
print "Order:";

 
FFTLength := 2^16;


NumberOfRuns := 10;

assert IsPowerOfTwoInteger(FFTLength);
assert (#F - 1) mod FFTLength eq 0;

W := ManualPrimitiveRoot(
    F,
    ManualRootValue,
    FFTLength
);

assert Order(W) eq FFTLength;

InputX := FibonacciSequenceInField(F, FFTLength);

print "FFT length:";
print FFTLength;

print "FFT root:";
print W;

print "Order of FFT root:";
print Order(W);


for i := 1 to NumberOfRuns do

    
    X := InputX;

    RunStartTime := Realtime();

    
    InterpolationStartTime := Realtime();

    
    IFFTWithRoot(~X, W);

    InterpolationElapsedTime :=
        Realtime(InterpolationStartTime);

    Append(
        ~InterpolationTimes,
        RealNumbers!InterpolationElapsedTime
    );

    
    ProgramWithoutInterpolationStartTime := Realtime();

    //===============================================================================================================================================//
    //==================================== Post-IFFT constraint and composition-polynomial construction ====================================================================//
    //===============================================================================================================================================//

    Z := Integers();
    N := #X;
    Number02 := 2;

    Pi := [ Z | Z!X[i] : i in [1..#X] ];
    delete X;
    Number04 := (p - 1) div N;

    
     GG := Z!W;

    error if Modexp(GG, N, p) ne 1,
        "gg is not an N-th root of unity";

    if N gt 1 then
        error if Modexp(GG, N div 2, p) eq 1,
            "gg does not have exact order N";
    end if;

    ArgX := [ Z | 0 : j in [1..N] ];

    for j := 0 to N - 1 do
        ArgX[j + 1] := Modexp(GG, j, p);
    end for;

    Tmp01 := [ Z | 0, 0, 0 ];

    Tmp01[1] := 1;
    Tmp01[2] := -(ArgX[N] + ArgX[N - 1]);
    Tmp01[3] := ArgX[N] * ArgX[N - 1];
    delete ArgX;
    for i := 0 to (N div 2) - 1 do
        Temp := Pi[N - i];
        Pi[N - i] := Pi[i + 1];
        Pi[i + 1] := Temp;
    end for;

    print "***** Post-IFFT part start *****";
    print "Polynomial f(x):";

    print "";

    FGX := [ Z | 0 : i in [1..N] ];
    FG2X := [ Z | 0 : i in [1..N] ];
    SubtractionArr := [ Z | 0 : i in [1..N] ];

    print "Polynomial f(g*x):";

    Counter02 := N - 1;

    for i1 := 0 to N - 1 do
        FGX[i1 + 1] := Pi[i1 + 1] * Modexp(GG, Counter02, p);
        FGX[i1 + 1] := FGX[i1 + 1] mod p;

        

        Counter02 -:= 1;
    end for;

    print "";

    print "Polynomial f(g^2*x):";

    Number03 := Modexp(GG, Number02, p);
    Counter03 := N - 1;

    for i := 0 to N - 1 do
        FG2X[i + 1] := Pi[i + 1] * Modexp(Number03, Counter03, p);
        FG2X[i + 1] := FG2X[i + 1] mod p;
 
       

        Counter03 -:= 1;
    end for;

    print "Polynomial f(g^2*x) - f(g*x) - f(x):";

    for i := 0 to N - 1 do
        SubtractionArr[i + 1] :=
            FG2X[i + 1] - FGX[i + 1] - Pi[i + 1];

        SubtractionArr[i + 1] :=
            SubtractionArr[i + 1] mod p;

        
    end for; 

    delete FGX;
    delete FG2X;
    delete Pi;
    print "Polynomial (x-a)*(x-b)*(f(g^2*x) - f(g*x) - f(x))";

    NewSize := N + 2;
    Mul := [ Z | 0 : i in [1..NewSize] ];

    for i := N - 1 to 0 by -1 do
        for j := 2 to 0 by -1 do
            Mul[i + j + 1] +:=
                SubtractionArr[i + 1] * Tmp01[j + 1];

            Mul[i + j + 1] :=
                Mul[i + j + 1] mod p;
        end for;
    end for;

    delete SubtractionArr;
    delete Tmp01;
    
    print "Polynomial (x-a)*(x-b)*(f(g^2*x) - f(g*x) - f(x))/(x^n - 1)";

    DegreeOfNumerator := NewSize;
    DegreeOfDivider := N;
    Rdeg := DegreeOfNumerator - DegreeOfDivider;

    Res := [
        Z |
        0 : i in [1..NewSize - N + 1]
    ];

    PolynomialDivider := [
        Z |
        0 : i in [1..DegreeOfNumerator]
    ];

    for i10 := 0 to DegreeOfNumerator - 1 do
        PolynomialDivider[i10 + 1] := 0;
    end for;

    PolynomialDivider[2] := 1;
    PolynomialDivider[DegreeOfNumerator] := p - 1;

    Counter04 := 1;

    for i11 := 0 to Rdeg - 1 do
        Res[Rdeg - i11] :=
            Mul[i11 + 1] div PolynomialDivider[2];

        printf "res [%o] = %o\n",
            i11, Res[Rdeg - i11];

        print "";

        for j4 := 0 to DegreeOfNumerator do
            Mul[j4 + i11 + 1] -:=
                PolynomialDivider[Counter04 + 1] *
                Res[Rdeg - i11];

            Counter04 +:= 1;

            if Counter04 eq NewSize then
                break;
            end if;
        end for;

        Counter04 := 1;
    end for;

    delete Res;
    delete Mul;
    delete PolynomialDivider;

    print "***** Post-IFFT part finished *****";
    print " TIME:";

    ProgramWithoutInterpolationElapsedTime :=
        Realtime(ProgramWithoutInterpolationStartTime);

    Append(
        ~ProgramWithoutInterpolationTimes,
        RealNumbers!ProgramWithoutInterpolationElapsedTime
    );

    ElapsedTime := Realtime(RunStartTime);

    

    Append(~RunTimes, RealNumbers!ElapsedTime);

   
    fprintf PerRunSectionOutputFile,
        "%o,%.9o,%.9o\n",
        i,
        InterpolationElapsedTime,
        ProgramWithoutInterpolationElapsedTime;

    Flush(PerRunSectionOutputFile);

    
    printf "Run %o of %o: interpolation time = %.9o seconds\n",
        i,
        NumberOfRuns,
        InterpolationElapsedTime;

    printf "Run %o of %o: program without interpolation time = %.9o seconds\n",
        i,
        NumberOfRuns,
        ProgramWithoutInterpolationElapsedTime;

    fprintf OutputFile, "%o,%.9o\n",
        i, ElapsedTime;

    Flush(OutputFile);

    printf "Run %o of %o: %.9o seconds\n",
        i, NumberOfRuns, ElapsedTime;

end for;

printf "Magma threads: %o\n",
    GetNthreads();

Flush(PerRunSectionOutputFile);
delete PerRunSectionOutputFile;




SumOfRunTimes := &+RunTimes;


AverageTime := SumOfRunTimes / #RunTimes;


AverageInterpolationTime :=
    &+InterpolationTimes / #InterpolationTimes;

AverageProgramWithoutInterpolationTime :=
    &+ProgramWithoutInterpolationTimes /
    #ProgramWithoutInterpolationTimes;




fprintf OutputFile, "sum_of_runs (without file creation and writing),%.9o\n", SumOfRunTimes;
fprintf OutputFile, "average (without file creation and writing),%.9o\n", AverageTime;

Flush(OutputFile);


SectionOutputFileName :=
    "average_interpolation_and_remaining_program_times.csv";

SectionOutputFile := Open(SectionOutputFileName, "w");

fprintf SectionOutputFile,
    "section,average_seconds,number_of_runs,fft_length\n";

fprintf SectionOutputFile,
    "interpolation,%.9o,%o,%o\n",
    AverageInterpolationTime,
    NumberOfRuns,
    FFTLength;

fprintf SectionOutputFile,
    "program_without_interpolation,%.9o,%o,%o\n",
    AverageProgramWithoutInterpolationTime,
    NumberOfRuns,
    FFTLength;

Flush(SectionOutputFile);
delete SectionOutputFile;


TotalProgramTime :=
    RealNumbers!Realtime(ProgramStartTime);

Append(
    ~TotalProgramTimes,
    TotalProgramTime
);

TotalProgramTimesSum :=
    &+TotalProgramTimes;

AverageTotalProgramTime :=
    TotalProgramTimesSum /
    NumberOfRuns;


fprintf OutputFile, "total_program (with file creation and writing),%.9o\n",
    TotalProgramTime;
fprintf OutputFile, "average_total_program (with file creation and writing),%.9o\n",
    AverageTotalProgramTime;
Flush(OutputFile);


delete OutputFile;



print "Number of runs :", NumberOfRuns;

printf "Average measured time per run (without file creation and writing): %.9o seconds\n",
    AverageTime;

printf "Sum of individual run times (without file creation and writing): %.9o seconds\n",
    SumOfRunTimes;

printf "Total program run time (with file creation and writing): %.9o seconds\n",
    TotalProgramTime;

printf "Average total program run time (including setup and file I/O): %.9o seconds\n",
    AverageTotalProgramTime;

/*
AverageTotalProgramTime is calculated as the total wall-clock time
of the complete program session containing 10 consecutive runs,
divided by NumberOfRuns (= 10).
It includes setup and file I/O and is not the arithmetic mean
of independently measured full-program executions.
*/


print "";
print "=== run_times.csv ===";
print "run,seconds";

for RunNumber := 1 to NumberOfRuns do
    printf "%o,%.9o\n",
        RunNumber,
        RunTimes[RunNumber];
end for;

printf "average,%.9o\n",
    AverageTime;

printf "sum_of_runs,%.9o\n",
    SumOfRunTimes;

printf "total_program,%.9o\n",
    TotalProgramTime;

printf "average_total_program,%.9o\n",
    AverageTotalProgramTime;



print "";
print "=== average_interpolation_and_remaining_program_times.csv ===";
print "section,average_seconds,number_of_runs,fft_length";

printf "interpolation,%.9o,%o,%o\n",
    AverageInterpolationTime,
    NumberOfRuns,
    FFTLength;

printf "program_without_interpolation,%.9o,%o,%o\n",
    AverageProgramWithoutInterpolationTime,
    NumberOfRuns,
    FFTLength;

