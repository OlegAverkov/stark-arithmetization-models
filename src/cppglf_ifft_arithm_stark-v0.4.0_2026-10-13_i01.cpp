//cppglf_ifft_arithm_stark-v0.4.0_2026-10-13_i01.cpp 
#include <NTL/ZZ_p.h>
#include <NTL/vector.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <vector>
#include <iostream>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <NTL/version.h>
#include <iostream>
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <psapi.h>

#pragma comment(lib, "Psapi.lib")

NTL_CLIENT
using namespace NTL;
using namespace std;
using namespace std;
using namespace chrono;
using namespace std;
using namespace std;

//================Goldilocks  prime field parameters=======================
const ZZ P = power(ZZ(2), 64) - power(ZZ(2), 32) + 1;
const ZZ FIELD_PRIMITIVE_ELEMENT = conv<ZZ>(7);
const ZZ FULL_FIELD_GENERATOR = conv<ZZ>(7);
ZZ invNlenVIglob;
//ZZ g =conv<ZZ>("9713644485405565297");// 2^24 input
//ZZ g = conv<ZZ>("5456943929260765144"); // 2^25 input
//ZZ g = conv<ZZ>("16905767614792059275"); // 2^23 input
//ZZ g = conv<ZZ>("5416168637041100469"); // 2^22 input
//ZZ g = conv<ZZ>("17654865857378133588"); //  2^21 input
//ZZ g = conv<ZZ>("3511170319078647661"); //  2^20 input
//ZZ g = conv<ZZ>("18146160046829613826"); //  2^19 input
//ZZ g = conv<ZZ>("9306717745644682924"); //  2^18 input
//ZZ g = conv<ZZ>("12380578893860276750"); //  2^17 input
ZZ g = conv<ZZ>("6115771955107415310"); //  2^16 input

ZZ number00 = conv < ZZ>("0");
ZZ number01 = conv < ZZ>("1");
ZZ number02 = conv < ZZ>("2");

const unsigned int RunTimes = 10;

//static_assert to check whether high_resolution_clock is actually aliased to a real clock 
//https://stackoverflow.com/questions/38252022/does-standard-c11-guarantee-that-high-resolution-clock-measure-real-time-non
static_assert(
    std::is_same<high_resolution_clock, steady_clock>::value
    || std::is_same<high_resolution_clock, system_clock>::value,
    "high_resolution_clock IS NOT aliased to one of the other standard clocks!");
using maxres_sys_or_steady =
std::conditional<
    system_clock::period::den <= steady_clock::period::den,
    system_clock, steady_clock
>::type;
using maxres_nonsleeping_clock =
std::conditional<
    high_resolution_clock::is_steady,
    high_resolution_clock, maxres_sys_or_steady
>::type;



void initField() {
    ZZ_p::init(to_ZZ(P)); 
}


ZZ_p nthRootOfUnity(unsigned long long n) {
    
    
    if (n <= 0) {
        throw invalid_argument("n must be positive");
    }

    if (rem(P - 1, n) != 0) {
        throw invalid_argument("n must divide P - 1");
    }

    ZZ exponent = (P - 1) / n;

    ZZ_p root = power(
        to_ZZ_p(FIELD_PRIMITIVE_ELEMENT),
        exponent
    );

    return root;

}


void FFT(Vec<ZZ_p>& x, const ZZ_p& W) {
    long N = x.length();

    if (N <= 1) {
        return;
    }

    long HalfN = N / 2;

    Vec<ZZ_p> even, odd;
    even.SetLength(HalfN);
    odd.SetLength(HalfN);

    for (long i = 0; i < HalfN; ++i) {
        even[i] = x[2 * i];
        odd[i] = x[2 * i + 1];
    }

    
    ZZ_p WNext = W * W;

    FFT(even, WNext);
    FFT(odd, WNext);

    ZZ_p Wk = to_ZZ_p(1);

    for (long k = 0; k < HalfN; ++k) {
        ZZ_p T = Wk * odd[k];

        x[k] = even[k] + T;
        x[k + HalfN] = even[k] - T;

        Wk *= W;
    }
}


void IFFT(Vec<ZZ_p>& x, const ZZ_p& W) {
    long N = x.length();

    ZZ_p WInverse = inv(W);

    FFT(x, WInverse);

    ZZ_p NInverse = inv(to_ZZ_p(N));

    for (long i = 0; i < N; ++i) {
        x[i] *= NInverse;
    }
}


ZZ invNlenVI(ZZ N) {
        ZZ invElofGroup = conv<ZZ>("1");
    ZZ temp = N;
    for (ZZ ii = conv<ZZ>(1); ii < P; ii++)
    {
        ZZ antiDivider = temp * ii;
        ZZ antiDivider1 = antiDivider % P;
        if (antiDivider1 == 1)
        {
            invElofGroup = ii;
        }
    }
    
    return invElofGroup;
}


struct ProcessMemorySample
{
    SIZE_T CurrentPrivateBytes;
    SIZE_T PeakPrivateBytes;
    SIZE_T CurrentWorkingSetBytes;
    SIZE_T PeakWorkingSetBytes;
};


ProcessMemorySample ReadProcessMemory()
{
    PROCESS_MEMORY_COUNTERS_EX MemoryCounters{};
    MemoryCounters.cb =
        sizeof(PROCESS_MEMORY_COUNTERS_EX);

    const BOOL Result =
        GetProcessMemoryInfo(
            GetCurrentProcess(),
            reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(
                &MemoryCounters
                ),
            sizeof(MemoryCounters)
        );

    if (!Result)
    {
        throw std::runtime_error(
            "GetProcessMemoryInfo failed"
        );
    }

    return ProcessMemorySample{
        MemoryCounters.PrivateUsage,
        MemoryCounters.PeakPagefileUsage,
        MemoryCounters.WorkingSetSize,
        MemoryCounters.PeakWorkingSetSize
    };
}


void PrintProcessMemory(
    unsigned int RunNumber
)
{
    const ProcessMemorySample Memory =
        ReadProcessMemory();

    constexpr double BytesPerMiB =
        1024.0 * 1024.0;

    const std::ios::fmtflags PreviousFlags =
        std::cout.flags();

    const std::streamsize PreviousPrecision =
        std::cout.precision();

    std::cout
        << std::fixed
        << std::setprecision(2);

    std::cout
        << "Private memory after run " 
        << RunNumber
        << ": "
        << Memory.CurrentPrivateBytes / BytesPerMiB
        << " MiB\n";

    std::cout
        << "Peak private committed memory after run "
        << RunNumber
        << ": "
        << Memory.PeakPrivateBytes / BytesPerMiB
        << " MiB\n";

    std::cout
        << "Current working set after run " 
        << RunNumber
        << ": "
        << Memory.CurrentWorkingSetBytes / BytesPerMiB
        << " MiB\n";

    std::cout
        << "Peak working set after run " 
        << RunNumber
        << ": "
        << Memory.PeakWorkingSetBytes / BytesPerMiB
        << " MiB\n";

    std::cout.flags(PreviousFlags);
    std::cout.precision(PreviousPrecision);
}

int main() {
    
    const auto TotalProgramStartTime =
        steady_clock::now();
    const std::locale CsvLocale("ru_RU.UTF-8");

    
    ofstream inOut1(
        "CPP-STARK_radix-2-IFFT IFFT time.csv",
        ios::out | ios::trunc
    );

    ofstream inOut2(
        "CPP-STARK_radix-2-IFFT Post-IFFT time.csv",
        ios::out | ios::trunc
    );

    ofstream inOut3(
        "CPP-STARK_radix-2-IFFT TOTAL time.csv",
        ios::out | ios::trunc
    );

    inOut1.imbue(CsvLocale);
    inOut2.imbue(CsvLocale);
    inOut3.imbue(CsvLocale);

    if (!inOut1.is_open() ||
        !inOut2.is_open() ||
        !inOut3.is_open()) {

        cerr << "File was not opened successfully!"
            << endl;

        return 1;
    }

    inOut1 << "run;seconds\n";
    inOut2 << "run;seconds\n";
    inOut3 << "number_of_runs;total_seconds\n";

    inOut1.flush();
    inOut2.flush();
    inOut3.flush();

    for (unsigned int RunNumber = 0;
        RunNumber < RunTimes;
        ++RunNumber) {
        cout << "=== STARK arithmetization experiment: radix-2 IFFT ===" << endl;
      

        initField();

        Vec<ZZ_p> vi;
        Vec<ZZ> pi;
        
    //==================================================================================================// 
    //========================== Radix-2 inverse NTT over the Goldilocks field ====================================// 
    //==================================================================================================//   
       
    ZZ lenOfFibonacciSequence = power(ZZ(2), 16);
       
        ZZ N = conv<ZZ>(lenOfFibonacciSequence);
        unsigned long long lenOfFibonacciSequenceLong;

      
        stringstream ss;
        ss << lenOfFibonacciSequence;
        string str = ss.str();

        try {
            lenOfFibonacciSequenceLong = stoull(str);  

           
            vi.SetLength(lenOfFibonacciSequenceLong);  
            pi.SetLength(lenOfFibonacciSequenceLong);  
           
            cout << "Length of vector vi is: " << vi.length() << endl;
            cout << "Length of vector pi is: " << pi.length() << endl;
        }
        catch (const invalid_argument& ia) {
            cerr << "Invalid argument: " << ia.what() << endl;
        }
        catch (const out_of_range& oor) {
            cerr << "Out of range: " << oor.what() << endl;
        }

        

        cout << "Length N of Fibonacci array is:  " << lenOfFibonacciSequence << endl;
       
        cout << endl;
        vi[0] = conv < ZZ_p>("1");
        vi[1] = conv < ZZ_p>("1");
        for (int j = 2; j < lenOfFibonacciSequence; j++)
        {
            vi[j] = vi[j - 1] + vi[j - 2];
           
        }



        //===============================================================================================================================================//
        //====================================   Execution-trace interpolation  ============================================================================================//
        //===============================================================================================================================================//   
        cout << endl;
       
        ZZ_p W = to_ZZ_p(g);

        cout << "***** IFFT part start *****" << endl;
        cout << "The result of IFFT is: " << endl;
        const auto InterpolationStartTime =
            steady_clock::now();
        IFFT(vi, W);
        const auto InterpolationStopTime =
            steady_clock::now();

        const double InterpolationElapsedTime =
            duration<double>(
                InterpolationStopTime -
                InterpolationStartTime
            ).count();

        auto stop1 = high_resolution_clock::now();
        auto start2 = high_resolution_clock::now();
       
        
        cout << "***** IFFT part finished *****" << endl;
        cout << endl;
        const auto ProgramWithoutInterpolationStartTime =
            steady_clock::now();
        //===============================================================================================================================================//
        //==================================== Post-IFFT constraint and composition-polynomial construction ====================================================================//
        //===============================================================================================================================================//  
        for (int i = 0; i < vi.length(); i++) {
            pi[i] = rep(vi[i]);
        }

        ZZ number04 = (P - 1) / lenOfFibonacciSequenceLong;
        
        ZZ gg = g;
        if (PowerMod(gg, N, P) != 1) {
            throw runtime_error("gg is not an N-th root of unity");
        }

        if (PowerMod(gg, N / 2, P) == 1) {
            throw runtime_error("gg does not have exact order N");
        }
        ZZ* argX = new ZZ[lenOfFibonacciSequenceLong];
        for (int j = 0; j < lenOfFibonacciSequenceLong; j++)
        {
            argX[j] = PowerMod(gg, j, P);

        }
        ZZ tmp01[3]{};
        tmp01[0] = 1;
        tmp01[1] = (-(argX[lenOfFibonacciSequenceLong - 1] + argX[lenOfFibonacciSequenceLong - 2]));
        tmp01[2] = ((argX[lenOfFibonacciSequenceLong - 1] * argX[lenOfFibonacciSequenceLong - 2]));
        delete[] argX;
        for (int i = 0; i < lenOfFibonacciSequenceLong / 2; i++) {
            ZZ temp = pi[lenOfFibonacciSequenceLong - i - 1];
            pi[lenOfFibonacciSequenceLong - i - 1] = pi[i];
            pi[i] = temp;
        }



        cout << "***** Post-IFFT part start *****" << endl;
        cout << "Polynomial f(x): " << endl;
        
        cout << endl;

        ZZ* f_gx_ = new ZZ[lenOfFibonacciSequenceLong];
        ZZ* f_g2x_ = new ZZ[lenOfFibonacciSequenceLong];
        ZZ* subtractionArr = new ZZ[lenOfFibonacciSequenceLong];

        //cout << endl;
        cout << "Polynomial f(g*x):" << endl;
        int counter02 = lenOfFibonacciSequenceLong - 1;
        for (int i1 = 0; i1 < lenOfFibonacciSequenceLong; i1++) {
            f_gx_[i1] = pi[i1] * PowerMod(gg, counter02, P);
            f_gx_[i1] = f_gx_[i1] % P;
            
            counter02--;
        }
        cout << endl;
        

        cout << "Polynomial f(g^2*x):" << endl;
        ZZ number03 = PowerMod(gg, number02, P);
        int counter03 = lenOfFibonacciSequenceLong - 1;
        for (int i = 0; i < lenOfFibonacciSequenceLong; i++) {
            f_g2x_[i] = pi[i] * PowerMod(number03, counter03, P);
            f_g2x_[i] = f_g2x_[i] % P;
           
            counter03--;
        }
        // cout << endl;
        cout << "Polynomial f(g^2*x) - f(g*x) - f(x):" << endl;
        for (int i = 0; i < lenOfFibonacciSequenceLong; i++) {
            subtractionArr[i] = f_g2x_[i] - f_gx_[i] - pi[i];
            subtractionArr[i] = subtractionArr[i] % P;
            
        }
       
        delete[]f_gx_;
        delete[]f_g2x_;
        

        cout << "Polynomial (x-a)*(x-b)*(f(g^2*x) - f(g*x) - f(x))" << endl;
        const int NewSize = lenOfFibonacciSequenceLong + 2;
        ZZ* mul = new ZZ[NewSize];


        for (int i = lenOfFibonacciSequenceLong - 1; i >= 0; i--) {
            for (int j = 2; j >= 0; j--) {
                mul[i + j] += subtractionArr[i] * tmp01[j];
                mul[i + j] = mul[i + j] % P;

            }
        }
        delete[]subtractionArr;
        int i10 = 0;
        for (i10 = 0; i10 < NewSize; i10++) {
            
        }
        
        cout << "Polynomial (x-a)*(x-b)*(f(g^2*x) - f(g*x) - f(x))/(x^N - 1)" << endl;
        //========================================================================================
        //========================================================================================
        //========================================================================================
        
        const int degreeOfNumerator = NewSize;
        const int degreeOfDivider = lenOfFibonacciSequenceLong;
        int rdeg = degreeOfNumerator - degreeOfDivider;

        ZZ* res = new ZZ[NewSize - lenOfFibonacciSequenceLong + 1]{};
        ZZ* PolynomialDivider = new ZZ[degreeOfNumerator]{};

        for (int i10 = 0; i10 < degreeOfNumerator; i10++) {
            PolynomialDivider[i10] = 0;
        }

        PolynomialDivider[1] = 1;
        
        PolynomialDivider[degreeOfNumerator - 1] = P - 1;

        int counter04 = 1;

        for (int i11 = 0; i11 < rdeg; i11++) {
            res[rdeg - i11 - 1] = mul[0 + i11] / PolynomialDivider[1];
            cout << "res [" << i11 << "]  =" << (res[rdeg - i11 - 1]) << endl;
            //cout << (res[rdeg - i11 - 1]) << " ";
            cout << endl;
            for (int j4 = 0; j4 <= degreeOfNumerator; j4++) {
                mul[0 + j4 + i11] -= PolynomialDivider[counter04] * res[rdeg - i11 - 1];
                // cout << "mul [" << 0 + j4 + i11 << "]  =" << mul[0 + j4 + i11] % P << endl;
                counter04++;
                if (counter04 == NewSize) {
                    break;
                }

            }

            counter04 = 1;

        }
        delete[]res;
        delete[]mul;
        delete[]PolynomialDivider;
        cout << "***** Post-IFFT part finished *****" << endl;
        cout << " TIME: " << endl;
        const auto ProgramWithoutInterpolationStopTime =
            steady_clock::now();

        const double ProgramWithoutInterpolationElapsedTime =
            duration<double>(
                ProgramWithoutInterpolationStopTime -
                ProgramWithoutInterpolationStartTime
            ).count();
        PrintProcessMemory(RunNumber + 1);
        
        inOut1
            << RunNumber + 1 << ";"
            << fixed << setprecision(9)
            << InterpolationElapsedTime
            << "\n";

        inOut2
            << RunNumber + 1 << ";"
            << fixed << setprecision(9)
            << ProgramWithoutInterpolationElapsedTime
            << "\n";

        inOut1.flush();
        inOut2.flush();

        
        cout << fixed << setprecision(9);

        cout << "Run "
            << RunNumber + 1
            << " of "
            << RunTimes
            << ": IFFT time = "
            << InterpolationElapsedTime
            << " seconds"
            << endl;

        cout << "Run "
            << RunNumber + 1
            << " of "
            << RunTimes
            << ": Post-IFFT time = "
            << ProgramWithoutInterpolationElapsedTime
            << " seconds"
            << endl;

       
    }  

    std::cout
        << "NTL version: "
        << NTL_VERSION
        << std::endl;
    
    inOut1.flush();
    inOut2.flush();

    inOut1.close();
    inOut2.close();

    
    const auto TotalProgramStopTime =
        steady_clock::now();

    const double TotalProgramElapsedTime =
        duration<double>(
            TotalProgramStopTime -
            TotalProgramStartTime
        ).count();

    
    inOut3
        << "number_of_runs;"
        << RunTimes
        << "\n";

    inOut3
        << "total_seconds;"
        << fixed << setprecision(9)
        << TotalProgramElapsedTime
        << "\n";

    inOut3.flush();

    cout << fixed << setprecision(9);

    cout << "Total continuous program time for "
        << RunTimes
        << " runs: "
        << TotalProgramElapsedTime
        << " seconds"
        << endl;

    inOut3.close();


    
    return 0;
}
