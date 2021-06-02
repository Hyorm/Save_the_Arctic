# Save_the_Arctic
Computer_Graphics, OpenGL_Project<br>
Background Study: [OpenGL_Mac](https://github.com/Hyorm/OpenGL_Mac)
## Dependent Libraries
1. freeglut
2. glew32
3. glm
4. ode
5. tiny_obj_loader

## Summary
### Program Description
Currently, polar bears are having a lot of difficulties. Threats such as global warming, reckless fishing, and oil drilling are destroying the Arctic's natural environment. As you can see figure 1, you can see the melting of sea ice and the polar bear losing its place to live. Due to this situation, polar bears are having a hard time living. So I made this game to let people know about this situation.
![image](https://user-images.githubusercontent.com/28642467/104836942-b19c3100-58f4-11eb-91b2-a3078b51a761.png)
<p align="center">Figure 1. factors that threaten the survival of polar bears</p>

#### Sysnopsis
The polar bear, which was living in peace, lost its place due to the danger that occurred one day. The story is that polar bears must survive these dangers. 
![image](https://user-images.githubusercontent.com/28642467/104837142-01c7c300-58f6-11eb-9dfc-cfd644f80144.png)
<p align="center">Figure 2. Prologue of this Game</p>

### Program Design

#### Usecase Diagram
<p align="center"><img width="800" alt="image" src="https://user-images.githubusercontent.com/28642467/104836872-3dfa2400-58f4-11eb-8348-56eda589cb59.png"></p>
<p align="center">Figure 3. Usecase diagram this Game</p>

The user changes the screen using the left mouse button click. When the game starts, the user uses directional keyboard input to move the polar bear character to avoid obstacles. When the game is over due to the timer or obstacle of the game, two additional screens will be displayed.

### Game Overview
<p align="center"><img width="300" alt="image" src="https://user-images.githubusercontent.com/28642467/104837222-71d64900-58f6-11eb-9869-2fa422df9c17.png"></p>
<p align="center">Figure 4. Information Cube of Game</p>

Applications pass information to users through textures mapped to the cube. When the game starts, the following screens will be displayed:

<p align="center"><img width="300" alt="image" src="https://user-images.githubusercontent.com/28642467/104837230-8286bf00-58f6-11eb-8328-5f1536b91588.png"></p>
<p align="center">Figure 5. User view with Ice(Right side)</p>

<p align="center"><img width="300" alt="image" src="https://user-images.githubusercontent.com/28642467/104837236-874b7300-58f6-11eb-9943-c78c58ce4996.png"></p>
<p align="center">Figure 6. User view with Oil(left side)</p>

<p align="center"><img width="300" alt="image" src="https://user-images.githubusercontent.com/28642467/104837239-8a466380-58f6-11eb-844e-672e865785a9.png"></p>
<p align="center">Figure 7. Developer view - Whole Map </p>

On the right side of the polar bear are carved ice cubes. On the left side of the polar bear, the oil spill threatens its survival. In front of the polar bear, indiscriminate fishing takes place, threatening its survival with guns. 

### Usage
- Compile with Visual Studio 2019    
- The user manipulates the game through the direction key, and when the game is over, the message that we want to deliver appears with the end.

### Result
[Youtube](https://youtu.be/f64oH4Hmx6E)
