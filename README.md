# `cvc5` Proof Decoder

Satisfiability Modulo Theories (SMT) solvers, such as `cvc5`, handle a variety of theories for
deciding the satisfiability of first-order logical formulas. Their efficiency and flexibility have made
them central in fields like program verification, testing, and model checking. However, the proofs
generated for unsatisfiable results are often very long and the reasoning is not easy to follow.
This thesis introduces a decoder that converts `cvc5`'s default proof format into a clearer and more
user-friendly version while preserving the logical correctness. The decoder is built in a modular
way, with separate steps for refactoring the proof, parsing it into its components, simplifying the
logical expressions, and finally formatting the output.

## Installation

### **Install `cvc5`**

Install `cvc5` version 1.2.1 following the installation steps descriped at https://github.com/cvc5/cvc5/blob/main/INSTALL.rst.

### **Clone the Repository**

Run the following command in a terminal:

```
git clone https://github.com/ooezenoglu/cvc5-proof-decoder.git
```

### **Install Flex & Bison**

Install Flex version 2.6.4 and Bison version 3.8.2:

```
sudo apt-get install flex
sudo apt-get install bison
```

Verify the installation and the correct version using the commands `flex --version` and `bison --version`.

### **Compile and run the program**

Execute the following command in the terminal adding from the below list flags:

```
make && ./cvc5-interpreter
```

| **Flag**         | **Description**                                                                            |
|------------------|--------------------------------------------------------------------------------------------|
| `--p` (optional) | Parse the TPTP problem given in `--f` into SMT-LIB format                                    |
| `--c`            | Required when `--p` is set; Relative path to the `cvc5` parser                              |
| `--d` (optional) | Calls the `cvc5` solver and decodes the proof                                                |
| `--r`            | Required when `--d` is set; Relative path to the `cvc5` solver                              |

For example, a TPTP problem can be translated into an SMT-LIB problem (by the `cvc5` parser), executed by the `cvc5` solver and finally the proof decoded with the following command:

```
make && ./cvc5-interpreter --p --f <PATH-TPTP-PROBLEM> --c <PATH-cvc5-PARSER> --r <PATH-cvc5-SOLVER> --d
```