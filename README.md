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

The repository is being prepared to include:

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

The repository provides source code and available experimental materials for independent evaluation of the implementations.

Information about tested software versions, input parameters, hardware configurations, and timing results will be added where available alongside the corresponding code versions and publications.

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
