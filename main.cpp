#include "kernel.h"

static void print_usage(const char* prog) {
    printf("Usage: %s <dimensions> <population> <objectif:0-4> [seed] [mode]\n", prog);
    printf("  objectif : 0=Levy 1=Rastrigin 2=Rosenbrock 3=Griewank 4=Sphere\n");
    printf("  mode     : cpu (defaut, DE sequentiel)\n");
    printf("             gpu (DE CUDA base, 6 kernels)\n");
    printf("             gpu-fused (DE CUDA fusionne, kernel IE + kernel(P) + kernel MCER)\n");
    printf("             gpu-opt (DE CUDA optimise : memoire partagee + streams + occupancy ;\n");
    printf("                      ne supporte pas objectif=0/Levy)\n");
    printf("  contrainte DE/rand/1 : population >= 4\n");
}

int main(int argc, char** argv) {

    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    int dim = std::stoi(argv[1]);
    int pop = std::stoi(argv[2]);
    int obj = std::stoi(argv[3]);
    unsigned int seed = (argc >= 5) ? (unsigned int) std::stoul(argv[4])
                                     : (unsigned int) time(NULL);
    std::string mode = (argc >= 6) ? std::string(argv[5]) : std::string("cpu");

    if (dim <= 0) {
        fprintf(stderr, "Erreur : dimension doit être > 0.\n");
        return 1;
    }
    if (pop < 4) {
        // DE/rand/1/bin a besoin de r1, r2, r3 distincts et != i => NP >= 4.
        fprintf(stderr, "Erreur : population doit être >= 4 (contrainte DE/rand/1).\n");
        return 1;
    }
    if (obj < 0 || obj > 4) {
        fprintf(stderr, "Erreur : objectif doit être entre 0 et 4.\n");
        return 1;
    }
    if (mode != "cpu" && mode != "gpu" && mode != "gpu-fused" && mode != "gpu-opt") {
        fprintf(stderr, "Erreur : mode doit être 'cpu', 'gpu', 'gpu-fused' ou 'gpu-opt'.\n");
        return 1;
    }
    if (mode == "gpu-opt" && obj == 0) {
        fprintf(stderr, "Erreur : mode gpu-opt ne supporte pas objectif=0 (Levy). "
                         "Utilise cpu, gpu ou gpu-fused pour Levy.\n");
        return 1;
    }

    float* gBest = new float[dim];
    long long fes_used = 0;

    printf("Type \t\t Time \t\t Minimum \t\t FEs\n");

    const char* type_label;
    clock_t begin = clock();
    if (mode == "cpu") {
        srand(seed);
        run_de_sequential(dim, pop, obj, gBest, &fes_used);
        type_label = "DE-CPU";
    } else if (mode == "gpu") {
        cuda_de(dim, pop, obj, (unsigned long long) seed, gBest, &fes_used);
        type_label = "DE-GPU";
    } else if (mode == "gpu-fused") {
        cuda_de_fused(dim, pop, obj, (unsigned long long) seed, gBest, &fes_used);
        type_label = "DE-GPU-FUSED";
    } else {
        cuda_de_optimized(dim, pop, obj, (unsigned long long) seed, gBest, &fes_used);
        type_label = "DE-GPU-OPT";
    }
    clock_t end = clock();

    printf("%s \t %10.3lf \t %f \t %lld\n",
           type_label,
           (double)(end - begin) / CLOCKS_PER_SEC,
           de_fitness_function(gBest, dim, obj),
           fes_used);

    delete[] gBest;
    return 0;
}
