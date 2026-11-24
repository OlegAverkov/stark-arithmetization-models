# STARK Arithmetization Models

Research implementations of STARK arithmetization in C++/NTL and Magma, including Goldilocks-field FFT/IFFT, execution-trace interpolation, constraint construction, performance experiments, and reproducibility materials.

## Project status

This repository is under active development.

The implementations are research prototypes intended for experimental evaluation, reproducibility, and further development. They are not production-ready STARK proving systems and should not be used in security-critical applications.

## Scope

The project studies selected stages of STARK arithmetization over the Goldilocks prime field

$$
p = 2^{64} - 2^{32} + 1.
$$

The implemented workflow includes:

- construction of a Fibonacci execution trace;
- interpolation of the execution trace using radix-2 inverse FFT;
- manual or automatic initialization of roots of unity;
- construction of the transition constraint
  $f(g^2x) - f(gx) - f(x)$
- application of boundary factors;
- division by the subgroup vanishing polynomial;
- separate timing of interpolation and post-interpolation verification stages;
- experimental comparison of the C++/NTL and Magma implementations.

## Implementations

The repository includes::

- a C++ implementation based on NTL;
- a Magma implementation;
- experimental timing results;
- information about the software and hardware configurations used in the experiments;
- materials required to reproduce the reported results.

## Dependencies

### C++ implementation

The C++ implementation requires the NTL Number Theory Library.

NTL is an external dependency distributed under the GNU Lesser General Public License, version 2.1 or later:

https://libntl.org/

NTL itself is not included in this repository.

### Magma implementation

The Magma implementation requires a separately licensed installation of the Magma computer algebra system:

https://magma.maths.usyd.edu.au/

Magma itself, its binaries, installation files, and license keys are not included in this repository.

## Reproducibility

This repository contains the source code and experimental materials
used in the corresponding article.

### Source files

Files used for timing experiments:

- [`cppglf_ifft_arithm_stark-v0.3.1_2026-09-19_i01.cpp`](src/cppglf_ifft_arithm_stark-v0.3.1_2026-09-19_i01.cpp)
- [`mglf_ifft_arithm_stark-v0.12.0_2026-10-04_i01.m`](src/mglf_ifft_arithm_stark-v0.12.0_2026-10-04_i01.m)

The published Magma v0.12.0 timing file is a cleaned archival copy.
An obsolete memory-reporting block was removed because its output was
incorrect and was not used in the article. The removed block was
executed after all reported timing values had already been recorded
and therefore did not affect the timing results.

Files used for memory experiments:

- [`cppglf_ifft_arithm_stark-v0.4.0_2026-10-13_i01.cpp`](src/cppglf_ifft_arithm_stark-v0.4.0_2026-10-13_i01.cpp)
- [`mglf_ifft_arithm_stark-v0.13.0_2026-10-13_i01.m`](src/mglf_ifft_arithm_stark-v0.13.0_2026-10-13_i01.m)

Only the memory values produced by the dedicated memory-measurement
versions were used in the article.

Timing and memory measurements were performed under the software and
hardware configurations described in the article.

## Experimental data

The [`data`](data/) directory contains the experimental materials used
to evaluate and validate the C++/NTL and Magma implementations.

- [`data/timing-statistics`](data/timing-statistics/) contains the original
  timing workbooks. Each workbook reports ten consecutive runs for a
  particular implementation and input size `N`, including the measured
  IFFT, post-IFFT, and total runtimes, their mean values, and two-sided
  95% Student's t-confidence intervals.

- [`data/timing-results-validation`](data/timing-results-validation/)
  contains the processed timing results, regression and figure source
  data, comparisons between the implementations, and validation examples
  over small finite fields. It also includes a Mathcad worksheet with
  control calculations of FFT, IFFT, and composition polynomials.

- [`data/memory-statistics-results`](data/memory-statistics-results/)
  contains the peak resident-memory measurements for the tested input
  sizes and the source data used to compare the memory scaling of the
  C++/NTL and Magma implementations.

In these materials, `N` denotes both the execution-trace length and the
number of coefficients of the interpolation polynomial. The original
workbooks are preserved with their formulas, formatting, and original
working language.

Polynomial coefficient sequences must be compared using the same ordering
convention. The Mathcad worksheet lists coefficients in ascending order of
degree, whereas the C++/NTL and Magma programs print them in descending
order. For example, `[1, 7]` in Mathcad and `[7, 1]` in the program output
both represent the polynomial `7x + 1`.

### Terminology used in the workbooks

- `Середній час` / `Среднее время` — mean runtime;
- `Інтерполяція` / `Интерполяция` — interpolation stage performed using radix-2 IFFT;
- `Час роботи Інтерполяції` / `Время работы Интерполяции` — IFFT interpolation runtime;
- `Перевірка` / `Проверка` / `Verification` — post-IFFT stage. In these workbooks, this term does not mean verification of a complete STARK proof;
- `Загальний час` / `Общее время` — total measured runtime of the IFFT and post-IFFT stages for one run;
- `Довірчий інтервал` / `Доверительный интервал` — confidence interval;
- `Інтервал у відсотках` / `Интервал в процентах` — relative margin of error, expressed as a percentage of the mean runtime;
- `Вибіркова дисперсія` / `Выборочная дисперсия` — sample variance calculated using the divisor `r − 1`;
- `Кількість прогонів` / `Количество прогонов` — number of consecutive runs;
- `Кількість вхідних чисел Фібоначчі` / `Количество входных чисел Фибоначчи` — input size `N`, equal to the execution-trace length and the number of coefficients of the interpolation polynomial;
- `Тест №` / `Тест №` — test number;
- `Відсоток часу роботи всього алгоритму, який займає Інтерполяція` / `Процент общего времени, занимаемый Интерполяцией` — share of the total measured runtime spent in the IFFT interpolation stage;
- `Відсоток часу роботи всього алгоритму, який займає Верифікація` / `Процент общего времени, занимаемый Верификацией` — share of the total measured runtime spent in the post-IFFT stage;
- `Тип пам’яті` / `Тип памяти` — memory metric used for the measurement: Peak Working Set on Windows or `VmHWM` on Linux;
- `Peak Working Set` / `PeakWorkingSetSize` / `PeakWorkingSetBytes` — maximum amount of physical memory resident for the Windows process since its start;
- `VmHWM` — peak resident set size (resident-memory high-water mark) of the Linux process, as reported in `/proc/<pid>/status`;

## Publications

The corresponding research publications are currently in preparation.

Bibliographic references and DOI information will be added after publication.

## Citation

Citation metadata are provided in the [CITATION.cff](CITATION.cff) file.

Until the corresponding publications and versioned software releases are available, please cite the repository URL and the specific commit or release used in the experiment.

## Feedback

Questions, bug reports, reproducibility results, and suggestions are welcome.

Please use GitHub Issues to report problems or discuss the implementations.

## Author

Oleg Averkov  
V. N. Karazin Kharkiv National University

## License

The original source code in this repository is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

Third-party software and dependencies remain subject to their respective licenses.
