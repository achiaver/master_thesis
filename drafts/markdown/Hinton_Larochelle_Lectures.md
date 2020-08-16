# 12 C - Restricted Boltzmann Machines



1998 - Hinton came up with an idea to speed up learning time of a RBMs (short cut).

It has theoritical problems, but it works quite well in practice. It led to a rivival in BM learning.

In RBM it takes only one step to reach thermal equilibrium when visible units are clamped. It's possible to compute the exact probability that $h_{j}$ will turn one by sigmoid function $\sigma (x)$.

"Contrastive Divergence is not following the gradient of log-likelihood, but it works well" [Hinton].

The negative phase after only one full reconstruction is "wrong".

$\Delta \omega_{ij} \propto {\langle s_{i} s_{j} \rangle}_{data} - {\langle s_{i} s_{j} \rangle}_{model}$

1. First term is the storage term as in the Hopfield network.
2. Second term is term to get out of spurious minimum, this term tells how much unlearnig to do.

Negative phase is like an unlearning in Hopfield to get rid of spurious minimum.

The marginal probability of state vector $\mathbf{v}$, $P(\mathbf{v})$, is
$$
P(\mathbf{v}) = \frac{\sum_{\mathbf{h}}e^{-E(\mathbf{v}, \mathbf{h})}}{\sum_{\mathbf{u}}\sum_{\mathbf{k}}e^{-E(\mathbf{u}, \mathbf{k})}},
$$
where the numerator is decreasing the energy, for a state visible vector $\mathbf{v}$ clamped and it finds state hidden vector $\mathbf{h}$ that fits nicely with it in order to get a low energy (this is the positive phase). The denominator, negative phase, has a similar purpose but for the partition function, it is finding global configurations, combinations of visible and hidden states, that give low energy, and therefore are large contribuition to $Z$, having found those global configurations, it raises their energy so that they contribute less. First term makes bigger, second term makes small.



## Larochelle

$$
E(\mathbf{v}, \mathbf{h}) = -\mathbf{v}^{T}\mathbf{W}\mathbf{h} - \mathbf{b}^{T} \mathbf{h} - \mathbf{c}^{T} \mathbf{v}
$$

$$
P(\mathbf{v}, \mathbf{h}) = \frac{e^{-E(\mathbf{v}, \mathbf{h})}}{Z},
$$

where
$$
Z = \sum_{\mathbf{u}} \sum_{\mathbf{k}} e^{-E(\mathbf{u}, \mathbf{k})}.
$$
Thus,
$$
P(\mathbf{v}, \mathbf{h}) = \frac{1}{Z}\left( e^{\mathbf{v}^{T}\mathbf{W}\mathbf{h}} e^{\mathbf{b}^{T}\mathbf{h}} e^{\mathbf{c}^{T}\mathbf{v}} \right).
$$
Restricted the connections allowing only between visible-hidden, and there is no visible-visible and hidden-hidden connections.

##### Free Energy


$$
\begin{align}
P(\mathbf{v}) &= \sum_{\mathbf{h} \in {\{0,1\}}^{m}} P(\mathbf{v}, \mathbf{h}) = \frac{1}{Z} \sum_{\mathbf{h} \in {\{0,1\}}^{m}} e^{-E(\mathbf{v}, \mathbf{h})} \\
	&= \frac{1}{Z} \left(e^{\mathbf{c}^{T} \mathbf{v}}\right) \left(\sum_{\mathbf{h} \in {\{0,1\}}^{m}} e^{\left(\mathbf{v}^{T} \mathbf{W} + \mathbf{b}^{T}\right)\mathbf{h}} \right) \\
	&= \frac{1}{Z}\left(e^{\mathbf{c}^{T}\mathbf{v}}\right)\left(\prod_{j=1}^{m} \left[\sum_{h_{j} \in {\{0, 1\}}} e^{\left(\mathbf{v}^{T} \mathbf{W_{\cdot j}} + b_{j}\right)h_{j}}\right] \right) \\
	&= \frac{1}{Z}\left(e^{\mathbf{c}^{T}\mathbf{v}}\right)\left(\prod_{j=1}^{m} \left[1 +   e^{\left(\mathbf{v}^{T} \mathbf{W_{\cdot j}} + b_{j}\right)}\right] \right) \\
	&= \frac{1}{Z}\left(e^{\mathbf{c}^{T}\mathbf{v}}\right) e^{ln\left[\prod_{j=1}^{m} \left[1 +   e^{\left(\mathbf{v}^{T} \mathbf{W_{\cdot j}} + b_{j}\right)}\right] \right]} \\
\Rightarrow P(\mathbf{v}) &= \frac{1}{Z} e^{\mathbf{c}^{T} \mathbf{v} + \sum_{j=1}^{m} ln \left[1 + e^{\left(\mathbf{v}^{T}\mathbf{W}_{\cdot j} + b_{j}\right)}\right]} \label{eq:marginalprob-free-energy}
\end{align}
$$

From equation ($\ref{eq:marginalprob-free-energy}$), we have de free energy $F(\mathbf{v})$, 
$$
\begin{align}
F(\mathbf{v}) &= \mathbf{c}^{T}\mathbf{v} + \sum_{j=1}^{m}ln\left[1 + e^{\left(\mathbf{v}^{T} \mathbf{W}_{\cdot j} + b_{j}\right)}\right] \\
\Rightarrow P(\mathbf{v}) &= \frac{e^{-F(\mathbf{v})}}{Z}.
\end{align}
$$




Fits a distribution to dataset is because it can be easier to work with and it is also more general, it applies to every experiment of the same kind.