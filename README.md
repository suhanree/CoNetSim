# CoNetSim - COevolving NETwork SIMulation

by Suhan Ree

## Overview

This is a C++ library for coevolving network dynamics with discrete-time and link delays.
I built this library for my own research on network dynamics.
At first, I started to make classes for graph representations and their manipulations,
and used them on many possible network models and their simulations
(as an example, see a model of for opinion dynamics in social networks at 
https://github.com/suhanree/OpinionNetwork.git).

Then I decided to make the code public as an open source project. 
To do that, I built a library containing all the classes I made for network simulations,
and named it as CoNet, so that anybody can download the code and apply it to any network
model that can be represented by this library.

I briefly describe possible systems, for which this library can be used, here.

* A system should have separate entities interacting each other, forming a network.
In a network, separate entities are called **nodes**, while their relationships (or connections) are **links**.

* A system can be represented by many graph structures. 
For example, in a social network, there can be many types of coexisting relations like friendship, family, and so on.
Foe each different relationship, we can assign different graphs with different properties.
As a result, there can be many types of nodes and links in one system.

* Nodes and links can have their own states, and they can evolve with time interacting each other.
We assume the time advances discretely one timestep at a time.

* Link structures (basically the information of which nodes links are attached to) can also evolve with time,
and they can be influenced by node and link states.

* When nodes influence other nodes, they should be connected by links, and influence of one node can be delayed based
on the property of links that are connected to that node (called **link delay**). 
Each link will have its own link delay value (it should be at least one timestep).

At first glance, the system description seems simple, but if we go farther into the detail of dynamics, it can
be quite complicated: for example, when we update states of nodes and links at each timestep, the update order can
change the outcome of simulation.
So we have to be careful when we define dynamic rules of the system.

Lastly, I should mention that the code is not complete yet, and that lots of algorithmic improvements
can be made.
One of the reason I opened this code to public is for the reproducibility of my research. Anybody who is interested
in my work can download this code and reproduce results.
If this library can help others to solve their own problems, it would make me much happier.  
To compile this library, run 'make' at the root directory of this library ($CONET).
Then 'libconet.a' will be created at '$CONET/lib'.
There are some example codes that use this library, at '$CONET/examples'.
