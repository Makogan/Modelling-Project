# CPSC 589 Modelling, UCalgary, Faramarz Samavati
### Developed by: [Susant Pant](https://github.com/susant-pant), [Camilo Talero](https://github.com/Makogan), [Shannon Tucker-Jones](https://github.com/ShannonTJ)
## Course Project
The Procedural Generation of a residential house, starting with a basic floor plan derived from a room-expansion algorithm, as described in [Jess Martin's paper "Procedural House Generation: A method for dynamically generating floor plans."](http://axon.cs.byu.edu/Dan/673/papers/martin.pdf) We focused on the possibility of the user being able to manipulate and specify the output of the procedural method, so that the final result is aesthetically pleasing but much simpler to create than starting from scratch. Additions include: removing walls, moving room-locations, and shrinking and scaling rooms, with further additions remaining on customizing the space with furniture, doors and windows, each of which will have modifiable positions.

## Interaction
### Camera
__W, A, S, D__: movement

__Arrow Keys__: direction

__Numpad +, -__: change incline

__Numpad *__: reset view

__Numpad /__: reset camera

### The Rooms
Assume that at the start of the program, the top of the screen is North.

Martin's algorithm uses a method called 'expansion,' the process of which is visible in our program. At first, rooms are only a point. From there, they start expanding until they encounter other rooms, at which point they stop. If no rooms are encountered, the room will keep growing until its maximum area is reached, after which it stops by itself.

Expansion is always taking place, so that the slightest bare space in the rooms will cause the rooms to start expanding again, as much as they can. This is very useful in scaling operations, where making a room very small in a new space can cause it to 'snuggle' up to the edges of the space instead of being manually scaled to that space. If this is not desired, it is possible to toggle off the expansion as required. We recommend this for smaller scaling and movement operations. Toggling off expansion shows the perimeter of the house; a kind of 'outer wall' to wrap the rooms inside.

__Space Bar__: toggle expansion and perimeter display

__Holding and dragging a Node__: move the room associated with it

__Holding and dragging the top right corner of a room__: scale the room up or down by its top right position

__Holding and dragging the bottom left corner of a room__: scale the room up or down by its bottom left position

A Node is one of the black squares, each of which is associated with a room. The rooms start 'expanding,' as described by Martin, from that point. Notice that the graph that shows which rooms ought to be connected to each other is visible as black lines connecting the Nodes. The white squares are the intersections of the graph's lines and the walls, which are the initial locations of doors.

__Clicking on a Node__ selects it, and so doing selects its room.

__I, J, K, L__: delete the wall (of the selected room) associated with the button pressed. IJKL on a QWERTY Keyboard is synonymous with WASD, i.e. I = W = North, K = S = South, J = A = West, and L = D = East.

__Holding and Dragging a Node with Left CTRL held down__: move the Node without moving its associated room.

__Right CTRL__: toggles 3D walls (to better see which walls are deleted)

### UI
__Esc__: close the program