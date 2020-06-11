# About RBMs

[website reference](https://www.quora.com/How-do-I-use-RBM-Restricted-Boltzmann-Machine)

###### Reply given by - **[Aditya Sharma](https://www.quora.com/profile/Aditya-Sharma-240)**, Master's Data Science, The University of British Columbia (2019)



If you have already read about RBMs, you might know that RBMs have found applications in [dimensionality reduction](https://en.wikipedia.org/wiki/Dimensionality_reduction), [classification](https://en.wikipedia.org/wiki/Statistical_classification), [collaborative filtering](https://en.wikipedia.org/wiki/Collaborative_filtering), [feature learning, ](https://en.wikipedia.org/wiki/Feature_learning)and [topic modeling](https://en.wikipedia.org/wiki/Topic_model).

One example of an application can be a Recommendation System based on [Collaborative Filtering](https://en.wikipedia.org/wiki/Collaborative_filtering). This technique is collaborative in the sense that we can use the taste and choices of many users to create automatic predictions for a single user based on his/her taste or choice in the same domain. The underlying assumption of the collaborative filtering approach is that if a person *A* has the same opinion as a person *B*on an issue, A is more likely to have B's opinion on a different issue than that of a randomly chosen person.

It is easy to apply collaborative filtering using RBMs to a domain if you have the data to train your network on. At the least, you will need the data on some users who have given some score/rating to a bunch of examples from a domain (say movies). So if you have the three variables (user, movie, and rating) you are good to go forward and feed this data to your network (the RBM) which will help the network learn the latent factors from the data and in the reconstruction of the probability distribution of the input (an approximation obviously!). Now, you can use this trained model to get the scores/ratings for the movies that a particular user has not already watched and in this way, get the recommendations for this user.

I have created a [Book Recommendation System](https://github.com/adityashrm21/Book-Recommender-System-RBM) using a simple implementation of Restricted Boltzmann Machines on the [goodbooks-10k](https://github.com/zygmuntz/goodbooks-10k) dataset. Feel free to check it out and use it as a base for your own application of RBMs! Here is the link to my GitHub repo:

[adityashrm21/Book-Recommender-System-RBM](https://github.com/adityashrm21/Book-Recommender-System-RBM)

I have also explained a little bit of RBMs in the description of the repo for your reference. Hope this helps!