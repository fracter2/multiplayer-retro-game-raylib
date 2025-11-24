Final hand in for the "Network Programming for Games 1" course, 5SD808

Theodor Sven Sá Rydberg
theodor.rydberg@gmail.com
theodor-sven.sa-Rydberg.7663@student.uu.se


---- DESCRIPTION ----
This is my hand-in for the final assignment in the course Network Programming for Games 1.
It is a simple remake of Bomberman in Raylib/c++ with terrible gameplay but functioning multiplayer.
The network code is implemented using UDP in a server / client configuration.


---- HOW TO USE ----
The server and client can (and should) be run in visual studio using the debugger. 
Select either the server or client(meteor) project in the "configure startup projects" drop-down menu, and run. 
 - I have not been able to run them simultaneously from the same computer, probably because of issues with sockets, but I have not looked into it. 

WASD to move and SPACEBAR to place bombs.
Every other input uses numbers 1-5, the important ones displayed on screen.

For the server:
'1' Starts the server.
'2' Toggles sending broadcasts (default on. only way to join).
'3' Starts the match. Disables joining.
'4' Fills the lobby with bots (pre-match only, are replaced by incoming connections).
	- bots are controlled by server using the same WASD + SPACE, for easy debugging.


For the client:
'1' Quit match, gracefully (sends disconnect and awaits response).
'2' Print debug info (mid-game).


For both:
'5' (hold) pauses receiving packets (reads but ignores) to simulate packet loss


---- COLLABORATORS ----
I worked extensively with Sebastian Saastamoinen for debugging and initial prototyping. 
He has a separate repository that may include similar / the same code.

Naturally, as a part of a university assignment, I used code we made during the workshops and lessons as a basis.
No other people or AI have been involved.