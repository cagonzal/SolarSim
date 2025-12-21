C++ implementation of an N-body gravitational simulator using the leapfrog method. Currently focuses on simulating planetary motion in our solar system. 

Python scripts for visualizations. 

Governing equation solved is $a_i = \sum_{i\neq j} -G m_j (r_i - r_j) / |r_i - r_j|^3$ 

The leap frog method used solves 

$$ v^{k+1} = v^{k} + 2 \Delta t a^k $$ 
$$ x^{k+1} = x^{k} + 2 \Delta t v^k $$ 

The system is normalized such that 1 AU = 1, the solar mass $M_{sun}$ = 1, and 1 year = 1. This yields $G = 4\pi^2 AU^3 M_{sun}^{-1} year^{-2}$.
