# C rpg game
This repository was created with the intention of improving my knowledge in C. Feel free to send me your feedback.

> ⚠️ **Attention:** 1 - I haven't created any installation files yet (partly because the program isn't finished). Feel free to compile and test my game as much as you want!

> ⚠️ **Attention:** 2 - This code is initially only working in GNU/Linux environments. I'm not sure if it's working correctly for Windows. But, if you want to play, you can run the game using Docker (I wrote down the instructions at the end of the README)

```
 _________________________
|  |                   |  |
|[]|   Dungeon of The  |[]|
|  |      Mad Mage     |  |
|  |                   |  |
|  |                   |  |
|  |___________________|  |
|                         |
|    ________________     |
|    | __      |     |    |
|    ||  |     |     |    |
|    ||__|     |     |    |
|____|_________|_____|____|
```

---

## Libraries used
I created some libraries with the intention of not only adding to my project but also helping other Gnu/Lunux developers to carry out their projects. Feel free to check them out.

- [Tusk](https://github.com/LucasWerppFranco/Tusk-C-library)

---

## Run whith Docker
You can also use docker to run my game!

```
docker build -t rpg .
docker run -it --rm rpg
```
