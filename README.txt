Name: Billy Brickner

Guild Members: Billy Brickner, Coleman Hoyt, Amelia Atiles

Final Project
-The player must collect cans while not getting caught by the police; the more beers collected the more blurry the screen becomes.
USAGE:
Run: You run the exicutable in command line; Move the hero by using W and S; A and D to rotate the hero.

Controls:
    W - Move Forward
    S - Move Backwards
    A - Turn Left
    D - Turn Right

    Click + Mouse - Move Arcball
    Click + CTRL + Mouse - Zoom

    Note: Your move forward, but your heading changes every time you collide with the oreKart or the temple.

Compile Instructions: make
Bugs: Sometimes a tree spawns in front of the camera at the beginning, move the camera to see yourself.
No file format.
Billy: Billboarding, git conflicts, implementing textures, and rope physics   Amelia: Lights, FragementBuffer, and finding textures   Coleman: Cart heirarchy, fixed police rotation, and made sure the program worked on different platforms (lab and personal Apple computer)
32 man hours to complete the project.
8 there were some specific things that were not in the lab that were challenging.
7 because it required a lot of corrdination with members when we had other group projects and finals to study for.

Ruberic Requirement Descriptors:
Interactivity - Move around the world and collect beer.
Models w/ Heigherarchical animation - Orekart is wheels under a box in drawOreKart(). Balls are animated in Marble.cpp.
Two Texture Applications - Burring is done by performing caclulaitons on the FBO texture in the blurShader. The skybox uses mipmaps in the textureShader. We billboard trees and transparent fragements are discarded in billboardQuad.
Lighting - We have three lights that shine on an object in the middle. The light sources are located at the locations of the three beers. Each light has different material properties and colors and the work is done in the customShader.
Shaders - The first unique shader is the blur shader which provides disorientation to the user as they collect more beers in blurShader. The second unique shader is a phoneg shader with multiple inputs that shines several different lights on an object and sums their color value in customShader.
