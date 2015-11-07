# BabyWars
A simple turn-based strategy game created with cocos2d-x.
![Screenshot](https://github.com/Babygogogo/BabyWars/raw/master/Resources/Screenshot.png)

## Architecture Overview
### High-level system Architecture
There are three primary categories in the game:
* The application layer
This layer deals with the hardware and the operating system. Because the cocos2d-x engine has done most of the job for us, what we have to do is to encapsulate the details and provide suitable interfaces for other layers to use.
* The game logic layer
This layer manages the game state and how it changes over time. It also manages actors, which are the entities in the game.
* The game view layer
This layer presents the game state with graphics and sound. It also translates input into game commands that are sent to the game logic.

### Actor-component Architecture
Actors are the game entities. An actor itself can't do much thing. Instead, an actor is a container that contains various actor components and scripts, which determines the behavior of the actor.
For example, a unit actor may contains the following components/scripts:
* render component
* sound player component
* unit script

### File Organization
* \Classes (Contains most of the codes)
	* \BabyEngine (Contains codes that implements the general subsystems.)
	* \BabyWars (Contains game-specific codes such as actor components, events, controllers.)
* \proj.* (Contains platform-specific codes and project files. I'm developing the win32 version so I only use files in proj.win32.)
* (others) (These are the cocos2d-x files. Don't modify them.)

## Compiling and Running
### System requirements
* Visual Studio 2013 community or above
* Windows that can be installed onto the VS :)
* cocos2d-x v3.8.1

### Installing steps
1. Clone this repository to your local disk. I assume that you clone it to D:\BabyWars.
2. Create an empty cpp cocos2d-x project (in any directory you like).
3. Copy all the files in the project directory to D:\BabyWars WITHOUT overriding any existing files.
4. Launch VS and open D:\BabyWars\proj.win32\BabyWars.sln, then compile and run.

## Acknowledgements
The (future) features of the game are inspired by the Game Boy Wars series (and as you can see, I directly took some game resources from Game Boy Wars 2. I'm doing this only because I can't draw any of them...).

The architecture of the game is inspired by the Game Coding Complete, 4th edition, written by Mike McShaffry and Davie "Rez" Graham. This is a great book (although somewhat hard for me). And here is the web page of the game "TeapotWars" in the book: [TeapotWars](code.google.com/p/gamecode4). I've also exported the code to my github page: [TeapotWars](https://github.com/Babygogogo/gamecode4).

Thank you for visiting my repository and reading this!
