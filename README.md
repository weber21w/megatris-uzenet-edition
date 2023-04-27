# megatris-uzenet-edition
Megatris: Uzenet Edition

	This is a Uzenet enabled version of the original Megatris classic. The game has been converted to 60Hz gameplay, additional options have been added to in game menus, and compressed music has been applied to make room for features. Various fixes as have been suggested over the years, like garbage alignment, and adjustable input repeate delays. The main feature here is the ability to rapidly play a networked game of Tetris on Uzebox, which also implies they have access to the CPU opponent on the Uzenet server.

	Use should hopefully be clear. From the main menu you can select a 1 or 2 player local game(no network), or Uzenet. On booting the game will tell you if there is some missing component to make networking operate, and wont let you select UZENET if that is the case. Otherwise you will immediately jump into matchmaking and as soon as a player is available, you will begin to play them. During this wait time you will also be given the option to summon an CPU bot instead of waiting for another human player.

	The primary goal here is to take perhaps the quintessential classic Uzebox game, and demonstrate how to turn it into a working Uzenet game. Ideally others will see, learn, and adapt to their own implementations so that there are more Uzenet games. This also serves as a learning experience for the author, as I intend to put together an in depth networking tutorial which will describe different theories, problem patterns, and solutions to the latency problem.

	Several optimizations and rewrites of menus and other items have been done to graft in networking and gain back some resources. In the end, there is way more than enough resources(compressed music+inline mixer was big), and you could expand your own features onto this if you wished.
