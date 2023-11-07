# 3dstudio

<figure>
<img src="https://github.com/jeppe9821/3dstudio_engine/assets/86717334/3817bf48-84f6-4bc3-9e6a-9d3155820049.png" width="50%" height="50%"></img>
<br>
<figcaption><i>Figure 1 - system overview</figcaption></i>
<br>

</figure>


This C++ engine was built for the purpose of learning more about creating shaders in 3D environments. The rendering of the engine is built using the visitor pattern with the full system overview in Figure 1

The engine feature phong lightning, toon shaders, GPU particles, depth shading, multi texture blending, wireframe shading and skyboxes. It also contains scene loading from XML configurations and different camera types (orth, persp).

<figure>
<img src="https://github.com/jeppe9821/3dstudio_engine/assets/86717334/7d175263-7e6b-4d00-aaba-c6843aa9ede5.png" width="50%" height="50%"></img>
<br>
<figcaption><i>Figure 2 - performance test of 4 million particles</figcaption></i>
<br>
</figure>
Figure 2 showcases the particle system which is the final shader implemented into the engine
