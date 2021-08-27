![github-small](Gidim/Resources/Textures/Showcase1.png)

# Renderer features
* Easy and simple pipeline
* Automatic mesh construction of different predetermined geometric shapes (triangle, tetrahedron, cube, plane and sphere)
* Abstraction of compute shaders, structured buffers, constant buffers, UAVs and SRVs for ease of use
* Ability to generate normal map from displacement map or height map

# Boids scene
* GPU-based boids simulation
* Uses a dynamic hashed grid on the GPU to accelerate nearest neighbor search (explained here: [https://wickedengine.net/2018/05/21/scalabe-gpu-fluid-simulation/](https://wickedengine.net/2018/05/21/scalabe-gpu-fluid-simulation/))
* Bitonic sort was implemented for sorting boids based on cell IDs
* Threads within thread groups operate on boids spatially close to eachother to better utilize cache coherence
* An option was implemented to evaluate only even/odd boids every other frame. This gave a 2x speed increase at the expense of an accurate simulation. This option is disabled by default, but can be enabled in the compute shader.

![github-small](Gidim/Resources/Textures/Showcase3.png)

# FFT ocean scene
* Procedural ocean wave animations through GPU-based IFFTs
* Basic ocean shading (foam, reflection, refraction, specular)
* Procedural skybox using the Preetham sky model

![github-small](Gidim/Resources/Textures/Showcase2.png)
