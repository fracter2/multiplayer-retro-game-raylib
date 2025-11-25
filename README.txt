Final hand in for the "Network Programming for Games 1" course, 5SD808

Theodor Sven Sá Rydberg
theodor.rydberg@gmail.com
theodor-sven.sa-Rydberg.7663@student.uu.se


---- DESCRIPTION ----
This is my hand-in for the final assignment in the course Network Programming for Games 1.
It is a simple remake of Bomberman in Raylib/c++ with terrible gameplay but functioning multiplayer.
The network code is implemented using UDP in a server / client configuration.

Bombs explode in straight horizontal and vertical lines, even though there is no visual for it. 
Bombs also block movement.


---- HOW TO USE ----
The server and client can (and should) be run in visual studio using the debugger. 
Select either the server or client(meteor) project in the "configure startup projects" drop-down menu, and run. 
 - I have not been able to run them simultaneously from the same computer, probably because of issues with sockets, but I have not looked into it. 

WASD to move and SPACEBAR to place bombs.
Every other input uses numbers 1-6, the important ones displayed in-game.

For the server:
'1' Starts the server.
'2' Toggles sending broadcasts (default on. only way to join).
'3' Starts the match. Disables joining.
'4' Fills the lobby with bots (pre-match only, are replaced by players joining).
	- bots are controlled by server using the same WASD + SPACE.
'6' Toggle sending less game-state updates (forces interpolation on client side)

For the client:
'1' Quit match, gracefully (sends disconnect and awaits response).
'2' Print debug info (mid-game).


For both:
'5' (hold) pauses receiving packets (reads but ignores) to simulate packet loss


---- FURTHER DETAILS ---- 
The game logic runs at a fixed 60hz (aka 60 ticks/s). 
Network "receive" happens roughly as often as possible, while network "send" occurs every third tick, so 20hz.

The RTT is calculated by comparing the time a particular pck was sent, and the time it was acknowledged.
Because network "send" occurs at fixed intervals, the resulting RTT is always slightly lower than displayed.

While in an active match, "Input delay" refers to the delay between the current game-tick and the tick that 
the input from the player was pressed. So, if a player presses UP on game-tick 20, and the server receives & uses it
on the servers game-tick 25, the input delay is 5 game-ticks.

Clients predict their own movement, bomb and tilemap. 
To visualize this, the server-authoritative state is rendered underneath with a BLUE tint.


---- COLLABORATORS ----
I worked extensively with Sebastian Saastamoinen for debugging and initial prototyping. 
He has a separate repository that may include similar / the same code.

Naturally, as a part of a university assignment, I used code we made during the workshops and lessons as a basis.
No other people or AI have been involved.