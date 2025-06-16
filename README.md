\textbf{Goal} \\
Based on the same algorithm(Leapfrog），we want to know how much it can accelerate if we use CUDA instead of the basic CPU version and OpenMP version.\\

\textbf{Implementation}\\
-Parallel\\
       We use every  thread to compute the whole  acceleration of every particle\\
-Memory Management\\
       Info of particles are written on shared memory Body<Real, dim>* bodies, which avoids read and write global memory frequently. \\
       Use cudaMemcpy to copy data between Host and Device.\\
-*Tile Calculation\\
       Use shared memory to make sure all tiles collaborated.(not improved much)\\
\textbf{The energy stability of CUDA version\\}
\begin{figure}[H]
    \centering
    \includegraphics[width=0.5\linewidth]{Images/total energy.jpg}
    \caption{Energy scalability of CUDA version (using 100 bodies)}
\end{figure}
\textbf{Benchmark}\\
\begin{table}[h]
    \centering
    \begin{tabular}{|c|c|c|c|}
        \hline
        Condition& CUDA& OpenMP &Basic Version\\
        \hline
        input_size=100, T=1000,delta T=0.01& 853 ms& 30380 ms&110024 ms\\\hline
        input_size=100, T=1000,delta T=0.1			& 732 ms& 3126 ms&11506 ms\\\hline
        input_size=200, T=1000,delta T=0.01& 1014 ms& 108657 ms&458277 ms\\
        \hline
 input_size=200, T=1000,delta T=0.1& 2169 ms& 11170 ms	&46606 ms\\\hline
 input_size=4000, T=1000,delta T=0.01& 34240 ms& NA&NA\\\hline
    \end{tabular}\\\caption{Time Cost:   CUDA V.S OpenMP V.S. Basic Version}
    \label{tab:simple}
\end{table}
\end{itemize}
\textbf{Possible ways to improve}\\
1.Merge possible kernel\\
2.Using float4 instead of double\\
3.Loop unrolling technique\\
4.Using some CUDA function, like \texttt{\_fsqrt\_rn(x)} instead of \texttt{sqrt(x)} \\
5.Find proper tile size for huge number of particles\\
\textbf{Reference }\\
https://developer.nvidia.com/gpugems/gpugems3/part-v-physics-simulation/chapter-31-fast-n-body-simulation-cuda 
