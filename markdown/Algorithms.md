# CD-1

Single step of contrastive divergence can be summarized as follows:

1. Randomly select a training sample from training dataset, and assign activations to visible nodes, defining the visible vector $\mathbf{v}$.
2. Compute the probabilities of the hidden units and sample their activations generating hidden vector $\mathbf{h}$.
3. Compute outer product of $\mathbf{v}$ and $\mathbf{h}$, and call this *positive gradien*t.
4. From $\mathbf{h}$, reconstruct a visible vector $\mathbf{v'}$.
5. From $\mathbf{v'}$, reconstruct a hidden vector $\mathbf{h'}$ (Gibbs sampling step).
6. Compute outer product of $\mathbf{v'}$ and $\mathbf{h'}$, can call this *negative gradient*.
7. Update:
   1. Weight matrix $\mathbf{W}$ by factor $\Delta \mathbf{W} = \alpha (\textit{positive gradient} - \textit{negative gradient})$.
   2. Visible biases $\mathbf{c}$ by factor $\Delta \mathbf{c} = \alpha (\mathbf{v} - \mathbf{v'})$.
   3. Hidden biases $\mathbf{b}$ by factor $\Delta \mathbf{b} = \alpha (\mathbf{h} - \mathbf{h'})$.
8. Back to step (1), until whole training dataset has been used.



# PoE

Product of Experts is a technique that models a probability distribution by combining the output from several simpler distributions. 

The combination of several simpler probability distributions, know as *experts*, is the multiplication of their density functions. Each *expert* makes decisions based on a few dimensions instead of having to cover the full dimensionality of the problem.



# Simulated Annealing

1. Initialize the system configuration. Randomize $\mathbf{x}_{0}$.
2. Initialize $T$ with a large value.
3. Repeat until $T$ is small enough:
   1. repeat until the number of accepted transitions is below a threshold:
      1. Apply random perturbations $\mathbf{x}_{1} = \mathbf{x}_{0} + \Delta \mathbf{x}$
      2. Evaluate $\Delta E(\mathbf{x}) = E(\mathbf{x}_{0} + \mathbf{x}_{1}) - E(\mathbf{x}_{1})$:
         1. if $\Delta E(\mathbf{x}) < 0$, keep the new state;
         2. Otherwise accept the new state with probability $P = e^{-\Delta E / T}$.
   2. Set $T = T - \Delta T$.