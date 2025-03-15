#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// `Complex` is a type for complex numbers with double point precission
typedef struct { 
    double real; 
    double imag; 
} Complex;

// INIT-----------------------------------------------------------------------------

Complex* init(int n_qubits){
    size_t dim = 1 << n_qubits;  // 2^n_qubits
    Complex *state = malloc(dim * sizeof(Complex));
    if(!state){
        perror("[FATAL ERROR]: Not enough memory");
        exit(1);
    }

    // initialize state to |000...0>
    for (size_t i = 0; i < dim; ++i) {
        state[i].real = 0.0;
        state[i].imag = 0.0;
    }
    state[0].real = 1.0;  // amplitude 1 for |00..0>
    state[0].imag = 0.0;

    return state;
}

// HADAMARD-GATE-----------------------------------------------------------------------------

void apply_hadamard(Complex *state, int n, int k) {
    size_t dim = 1 << n;
    double inv_sqrt2 = 1.0 / sqrt(2.0);

    // loop over pairs of states: those with qubit k = 0 and k = 1
    size_t bit = 1 << k;  // mask for the k-th bit
    for (size_t i = 0; i < dim; i += 2 * bit) {
        for (size_t j = 0; j < bit; ++j) {
            size_t idx0 = i + j;       // state where qubit k is 0
            size_t idx1 = idx0 + bit;  // state where qubit k is 1

            // store original amplitudes
            Complex a = state[idx0];
            Complex b = state[idx1];

            // hadamard transform: (|0⟩ -> |0>+|1>)/sqrt(2), |1> -> (|0> - \1>)/sqrt(2)
            state[idx0].real = (a.real + b.real) * inv_sqrt2;
            state[idx0].imag = (a.imag + b.imag) * inv_sqrt2;
            state[idx1].real = (a.real - b.real) * inv_sqrt2;
            state[idx1].imag = (a.imag - b.imag) * inv_sqrt2;
        }
    }
}

// PAULI-GATES-----------------------------------------------------------------------------

void apply_pauli_x(Complex *state, int n, int k) {
    size_t dim = 1 << n;
    size_t bit = 1 << k;

    for (size_t i = 0; i < dim; i += 2 * bit) {
        for (size_t j = 0; j < bit; ++j) {
            size_t idx0 = i + j;
            size_t idx1 = idx0 + bit;

            Complex temp = state[idx0];

            state[idx0] = state[idx1];
            state[idx1] = temp;
        }
    }
}

void apply_pauli_y(Complex *state, int n, int k) {
    size_t dim = 1 << n;
    size_t bit = 1 << k;

    for (size_t i = 0; i < dim; i += 2 * bit) {
        for (size_t j = 0; j < bit; ++j) {
            size_t idx0 = i + j;
            size_t idx1 = idx0 + bit;

            Complex a = state[idx0];
            Complex b = state[idx1];

            // Y gate: |0> -> i|1> and |1> -> -i|0>
            // compute -i * b for idx0:
            state[idx0].real = b.imag;
            state[idx0].imag = -b.real;

            // compute i * a for idx1:
            state[idx1].real = -a.imag;
            state[idx1].imag = a.real;
        }
    }
}

void apply_pauli_z(Complex *state, int n, int k){
    size_t dim = 1 << n;
    size_t bit = 1 << k;

    for (size_t i = 0; i < dim; i += 2 * bit) {
        for (size_t j = 0; j < bit; ++j) {
            size_t idx0 = i + j;      // remains unchanged
            size_t idx1 = idx0 + bit;   // state where qubit k is 1

            // multiply amplitude at idx1 by -1.
            state[idx1].real = -state[idx1].real;
            state[idx1].imag = -state[idx1].imag;
        }
    }
}

// CNOT-GATE-----------------------------------------------------------------------------

void apply_cnot(Complex *state, int n, int control, int target) {
    size_t dim = 1 << n;
    size_t c_bit = 1 << control;
    size_t t_bit = 1 << target;

    for (size_t i = 0; i < dim; ++i) {
        // if control bit is 1 for index i, flip the target bit
        if (i & c_bit) {
            size_t j = i ^ t_bit;  // index j is i with target bit flipped
            if (j > i) {
                // swap amplitudes state[i] and state[j]
                Complex temp = state[i];
                state[i] = state[j];
                state[j] = temp;
            }
        }
    }
}

// MAIN-----------------------------------------------------------------------------

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage: `./pure-quanta #of_qubits`\n");
        return 1;
    }

    int qubits = atoi(argv[1]);
    if(qubits <= 0){
        printf("[FATAL ERROR]: Please input a positive number of qubits");
        return 1;
    }

    Complex* state = init(qubits);

    // temporary for now just insert gates below here to opperate

    for(size_t i = 0; i < 1 << qubits; i++){
        printf("real: %f\nimag: %f\n", state[i].real, state[i].imag);
    }

    
    free(state);
    return 0;
}