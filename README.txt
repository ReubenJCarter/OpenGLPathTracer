
To Build to release with VS, run cmake, open msbuild in build directory and type
msbuild ALL_BUILD.vcxproj /p:Configuration=Release



TODO
Add new types of image to image class
Add Textures to Renderer
(perhaps multipass (one color pass, one path tracing pass) break up rendering to allow way more texture to be rendered per scene)
cont.(could use rasaterisation to render color pass..)
Make BVH work correctly (ie faster than brute force)


NOTES

C = E0 + B0 * E1 + B0 * B1 * E2 + B0 * B1 * E3 ....

runningBRDF = 1,1,1

for bounce count
	Trace Ray
	Get Material at Hit
	Create New Ray
	Compute BRDF
	Color += material.Emmitance * runningBRDF
	runningBRDF *= BRDF
	Set Ray to new ray