** This README is unfinished and may contain outdated information **

# Introduction

Everything is implemented from scratch, including the matrix math, except the following external libraries:
`SDL2 v2.28.1` (https://github.com/libsdl-org/SDL/tree/release-2.28.1)  - Rendering
`Catch2 v3.4.0` (https://github.com/catchorg/Catch2/releases/tag/v3.4.0) - Testing
`nlohmann v3.11.2` (https://github.com/nlohmann/json/releases/tag/v3.11.2) - JSON parsing

# Summary on how to use

# Details on implementation and how to use

## Matrix

The `Matrix.h` and `Matrix.cpp` files are the header and implementation files for the `Mat` class which represents a matrix.

Internally, the represantation of the matrix is that of a 2-dimensional array (double**).

The matrix multiplication order is left-to-right.
This means that the number of columns in the left `Mat` must match the number of rows in the right `Mat`.
(i.e. if multiplying two vectors, the left vector must be a row vector (1xN), while the right one must be a column vector (Nx1))

The order of operations is: scaling -> rotation -> translation
i.e. `TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX * vector`

You can create a `Mat` instance by calling the initializer function with a 2-dimensional `std::initializer_list`, which is basically a 2-dimensional array, and will hold the rows and columns for the matrix, followed by the row dimension and column dimension.
Example for a `2x3` matrix: `Mat({{1, 2, 3}, {3, 4, 5}}, 2, 3)`

You can get a value from the matrix by calling the `get` `Mat` member function.
Example to get the value in the 2nd row and 3rd column: `matrix.get(2, 3)`

You can set a value in the matrix by calling the `set` `Mat` member function.
Example to set the value in the 2nd row and 3rd column to 5: `matrix.set(5, 2, 3)`

You can transpose a matrix by calling the `transposed` `Mat` member function.
Example: `matrix.transposed()`

You can, assuming that your matrices have valid dimensions, add (`+`), add and assign (`+=`), subtract (`-`), subtract and assing (`-=`), multiply by  (`*`), and multiply and assign (`*=`).

You can also perform element-wise operations using `scalar` values on any `Mat`, by multiplying (`*`), multiplying and assigning (`*=`), dividing (`/`), and dividing and assign (`/=`).

You can calculate the dot product between vectors by calling the `static` `Mat` function `dot`. This operation is commutative (i.e. order does not matter, neither does whether the vectors are row or column). The only requirements are that both must be vectors with the same number of elements (i.e. either the row dimension or column dimension must be `1`).
Example: `Mat::dot(vector_a, vector_b)`

You can calculate the vector norm (length) by calling the member `Mat` function `norm`.
Example: `vec_a.norm()`

You can normalize a vector by calling the `normalize` `Mat` member function. This divides the matrix, element-wise, by its norm/length. This also turns it into a unit vector (norm/length of 1).
Example: `matrix.normalize()`

You can test for equality and inequality, reassign a `Mat` instance to another `Mat` instance through copy & reassignment, or you can also directly reassign it to a new `Mat` instance.

You can print the matrix to the console by calling the `print` `Mat` member function.
Example: `matrix.print()`

You can also print it to the console by running `std::cout << matrix`.

## Engine.h and Engine.cpp

### Triangle
The smallest logical geometric figure is a triangle, which is implemented as the struct `Triangle` in `Engine.h`. A `Triangle` is simply an object that holds 3 vertices, which in the context of this program are 3 `4x1` vector (`Mat`) instances.

The `4x1` dimension is, for, respectively, the `x`, `y`, and `z` 2D/3D space coordinates of the vertex, as well as a 4th `w` dimension for aid in certain matrix operations, such as translation.

There are 2 ways in which you can instantiate a `Triangle`:
1. Calling the constructor `Triangle(vertex_a, vertex_b, vertex_c)`, where `vertex_a` is the `Mat` instantiated vector, and so on for the other vertices.<br>
2. Calling the constructor `Triangles(vertices)`, where `vertices` is an array of `3` vertex (`Mat`) instances (i.e. `const Mat[3] vertices`).

### Quad

The second smallest logic geometric figure is a quad, which is implemented as the struct `Quad` in `Engine.h`. A `Quad` is a superset of `Triangle` instances, it holds 2 triangles in order to form a quad. Physically it represents a geometric face, or 4 vertices.

There are 4 ways in which you can instantiate a `Quad`:
1. Calling the constructor `Quad(triangle_a, triangle_b)` with both being `Triangle` instances.<br>
2. Calling the constructor `Quad(triangles)` where `triangles` is a `Triangle` array of size `2` (i.e. `const Triangle[2] triangles`).<br>
3. Calling the constructor `Quad(vertex_a, vertex_b, vertex_c, vertex_d)`, where each one is a vertex `Mat` vector.<br>
4. Calling the constructor `Quad(vertices)`, where `vertices` is a `Mat` array of size `4`.

### Mesh
> The `obj` formatted meshes must be within the given `models_folder` folder, and the `mesh_filename` filename must be a valid file within the folder.<br>
> The `mesh_filename` must include the file extension.<br>
> The `models_folder` must have a `/` at the end, preferably an `absolute` path, otherwise you need to figure out the correct file tree. i.e. `models_folder = "D:/my_3d_models/"

The largest logic geometric figure is a `Mesh`, which is implemented in the `Engine.h` file.<br>

It contains information about an arbitrary 3D mesh, which can be loaded from an `.obj` file or hardcoded (by, for example, manually defining the vertex array and the face indices).

**The face indices start at `1`.**

A face can be either made up of `3` or `4` vertices, so a triangle is also considered a face.

Its member variables are the following:
`vertices`: An `std::vector` of type `Mat`, which stores all of the figure's vertices.<br>
`faces_indices`: An `std::vector` of type `std::vector<uint32_t>>`, which stores all the face indices.  The reason why there is a nested `std::vector` here is because each individual face is an individual `std::vector<uint32_t>` (an array of unsigned integers), which identify the respective vertex by indexing from the `vertices` array.<br>
`tex_coords`: (**Yet to be implemented**) The same behavior described above for `vertices`, except that this is meant for textures.<br>
`tex_indices`: (**Yet to be implemented**) The same behavior described above for `faces_indices`, except that this is meant for textures.<br>
`normals`: (**Yet to be implemented**) The same behavior described above for `vertices`, except that this is meant for normals.<br>
`normal_indices`: (**Yet to be implemented**)  The same behavior described above for `faces_indices`, except that this is meant for textures.<br>
`mesh_filename`: Stores the name of the file used for loading the mesh.<br>
`mesh_id`: Uniquely identifies the mesh. This value is defined as the number of total meshes (`total_meshes`), which is passed as an argument to the constructor, and should be the `Scene` member variable `total_meshes`, as it gets incremented by `1` before the function returns, and as it is passed by reference. This is so that we can keep track of how many meshes have been loaded and so that each one has an unique `mesh_id`.<br>

You are able to get the number of vertices from a `Mesh` by calling the member function `total_vertices`, or `total_faces` in the case of faces.

There are 2 ways you can instantiate a `Mesh`, and every time it happens, the `total_meshes` variable gets incremented by `1`:

1. Calling the constructor `Mesh(current_scene.total_meshes)`, which instantiates a hardcoded cube. (you can find specific coordinates and other numbers, such as the `width`, `height`, and `depth` within the function)
	> The cube is instantiated by storing each vertex in the `vertices` array and for every 4 vertices/1 face, the face index is also stored in `faces_indices`.<br>
	> You can find specific coordinates and other numbers such as the `width`, `height`, and `depth` of the cube within the function definition.<br>
	> The `mesh_filename` will be set to `cube.obj`.
2. Calling the constructor `Mesh(model_path, mesh_filename, current_scene.total_mesh)`, where `model_path` is the relative or absolute path to the `.obj` formatted mesh folder, and `mesh_filename` is the actual mesh's filename, including the extension, and `current_scene.total_mesh` keeps track of how many meshes have been loaded so far and also increments it by `1`.

### Instance

An instance is self-explanatory, it is simply an instance of some loaded `Mesh`. This is so that the same `Mesh` doesn't need to be loaded multiple times, and so that you can place it in the [Scene](#Scene) many times over, where each `Instance` has unique properties, such as their orientation and `world coordinates`.

Whenever you want to place an object in a scene, you first need a [Mesh](#Mesh), then you can have however many `Instance`s of that `Mesh` as you want, and each of them will have their own transformations applied to them, through the `MODEL_TO_WORLD` matrix.

Its member variables are the following:
`instance_name`: You can give a name to an instance. This means that, for example, you could load an arm `Mesh`, and have two `Instance`s, one where the arm is rotated to look like the left arm, and one that looks like the right arm, and use this to help you identify which is which. A `scene graph` would make perfect use of this and is something I would like to implement eventually. (It's basically a visual gui "list" where you can create models, section them in layers, groups, name them, combine them to make them it one model, etc.)<br>
`instance_id`: Uniquely identifies the `Instance`. Behaves the same as the `mesh_id`, except it is used for instances.<br>
`mesh`: A pointer to the `Mesh` which will be rendered. It holds the actual vertex (and soon to be implemented texture & normal) information.<br>
`MODEL_TO_WORLD`: A `4x4` matrix (`Mat`) instance, which is initialized as the identity matrix. It will hold the transformations for the given instance, and is used for the transition from `model space` into `world space`. (i.e. rotation, translation, scaling)<br>
`show`: A `bool` toggle for whether the `Instance` will be rendered or not.

There are 2 ways you can instantiate an `Instance`:
1. Calling `Instance(mesh, MODEL_TO_WORLD, show, total_instances)`, where `mesh` is a pointer to the `Mesh` (`Mesh* mesh`), `MODEL_TO_WORLD` is the model-to-world `Mat` instance, `show` is a boolean of whether or not the `Instance` should be rendered, and `total_instances` is the `Scene` member variable `current_scene.total_instances`, which keeps track of how many instances have been instantiated.<br>
2. Calling `Instance(instance_name, mesh, MODEL_TO_WORLD, show, total_instances)`, which is the same as above, except that you can also give the `Instance` a name directly, through the variable `instance_name`.

### Scene
> The `Scene` configuration file must be in `json` format and within the given `scene_folder`, the `scene_filename` must be a valid file within the folder as well.<br>
> The `scene_filename` must include the file extension.<br>
> The `scenes_folder` must have a `/` at the end, preferably an `absolute` path, otherwise you need to figure out the correct file tree. i.e. `models_folder = "D:/my_scenes/"

A `Scene` instance holds information about the scene to be rendered. This includes the meshes present in the scene, as well as the instances and other relevant information.

You can get a `copy` of a `Mesh` by calling `scene.get_mesh(mesh_id)`, where `scene` is the variable that instantiates your `Scene`, and `mesh_id` is the id of the `Mesh` you want.
You can use the mesh filename (including extension) instead of the mesh id.

You can get a `pointer` to a `Mesh` by calling `scene.get_mesh_ptr(mesh_id)`.
Similarly, you can do the same with the `mesh_filename` instead of the `mesh_id`.

Its member variables are:
`total_meshes`: Keeps track of the number of loaded meshes.<br>
`total_instances`: Keeps track of the number of instances.<br>
`total_triangles`: Keeps track of the number of triangles.<br>
`total_vertices`: Keeps track of the number of vertices.<br>
`rendered_meshes`: Keeps track of the number of rendered meshes<br>
`rendered_instances`: Keeps track of the number of rendered instances<br>
`rendered_triangles`: Keeps track of the number of rendered triangles<br>
`rendered_vertices`: Keeps track of the number of rendered vertices<br>
	> * The `rendered` here refers to those instances where the `show` member variable is `true`.
`scene_meshes`: An `std::deque` of type `Mesh` which holds the scene `Mesh` instances.<br>
`scene_instances`: An `std::deque` of type `Instance` which holds the scene `Instace` instances.<br>
`scene_name`: Stores the name of the `Scene`, which as of right know is unused, but can be useful eventually for identifying individual scenes.<br>
`scene_filepath`: Stores the filepath of the `json` formatted `Scene` configuration file .<br>
`scene_data`: The scene `JSON` data will be stored here. The `nlohmann::json` is a JSON parser from the external library `nlohmann`.

The `Scene` instances can be saved to and loaded from configuration files, in `json` format, through the `Scene` member functions `save_scene` and `load_scene`, respectively.

The arguments for `save_scene` (implemented in `Engine.cpp`) are:
`scenes_folder`: The folder in which the scene `json` formatted configuration file will be stored.<br>
`scene_filename`: The name of the output `json` formatted configuration file, excluding the extension.<br>
`models_folder`: The folder used for the models within the scene. (Where they currently are, not where you want them to be)
`verbose`: 

The arguments for `load_scene` (implemented in `Engine.cpp`) are:
`scenes_folder`: The folder where the `Scene` `json` formatted configuration file you will be loading is stored at. It can be relative or absolute.<br>
`scene_filename`: The filename (including the extension) of the `Scene` configuration file, it must be within the `scenes_folder` folder.<br>
`models_folder`: The folder where the `obj` formatted models are stored, it can be relative or absolute.<br>
`verbose`:  Useless as of right now, but can be used for omitting informationg when not debugging through the console.<br>
`camera_position`: A `4x1` vector `Mat` instance that has the position of the camera in `world space`. The first 3 components are the `x`, `y`, and `z` coordinates, respectively. The 4th dimension is always `1` and solely used to simplify matrix operations.<br>
`camera_direction`: A `4x1` vector `Mat` instance that has the direction (vector) of where the camera is pointing toward. The first 3 components are the `x`, `y`, and `z` coordinates, respectively. The 4th dimension is always `1` and solely used to simplify matrix operations.<br>
`camera_yaw`: A `double` representing the camera `yaw` in `world space` from the default orientation.<br>
`camera_pitch`: A `double` representing the camera `pitch` in `world space` from the default orientation.<br>
`camera_roll`: A `double` representing the camera `roll` in `world space` from the default orientation.<br>
`VIEW_MATRIX`: A `4x4` matrix `Mat` instance that represents the `view` matrix, which is responsible for dealing with the camera related transformations/movement.<br>

The `Scene` can be instantiated by calling the empty constructor `Scene()`, or by indirectly calling the `load_scene` function to load a `json` formatted configuration file, by passing the exact same arguments to the constructor: i.e. `Scene(scenes_folder, scene_filename, models_folder, verbose, camera_position, camera_direction, camera_yaw, camera_pitch, camera_roll, VIEW_MATRIX)`

This is how they are formatted:

**scene.json**
```
{
	"scene": "default_scene",
	"models": {
		"cube.obj": {
			"instances": {
				"cube_001": {
					"show": true,
					"translation": {
						"x": 0.25,
						"y": 0.25,
						"z": 0.25
					},
					"scale": {
						"x": 0.3,
						"y": 0.3,
						"z": 1
					},
					"rotation": {
						"x": 30,
						"y": 30,
						"z": 30
					},
					"model-to-world": [
						0.225,
						0.0649519,
						-0.1875,
						0.25,
						-0.129904,
						0.2625,
						-0.0649519,
						0.25,
						0.5,
						0.433013,
						0.75,
						0.25,
						0,
						0,
						0,
						1
					]
				},
				"cube_002": {
					"show": true,
					"translation": {
						"x": 2.22,
						"y": 1.5,
						"z": 3.4
					},
					"scale": {
						"x": 0.5,
						"y": 0.5,
						"z": 1
					},
					"rotation": {
						"x": 2,
						"y": 2,
						"z": 2
					}
				}
			}
		},
		"Cybertruck.obj": {
			"instances": {
				"Car": {
					"show": false,
					"translation": {
						"x": 0.25,
						"y": 0.25,
						"z": 0.25
					},
					"scale": {
						"x": 0.3,
						"y": 0.3,
						"z": 0.3
					},
					"rotation": {
						"x": -5,
						"y": -5,
						"z": -5
					}
				},
				"Cybertruck_002": {
					"show": false,
					"translation": {
						"x": 0.25,
						"y": 0.25,
						"z": 0.25
					},
					"scale": {
						"x": 0.3,
						"y": 0.3,
						"z": 1
					},
					"rotation": {
						"x": 5,
						"y": 5,
						"z": 5
					}
				}
			}
		}
	}
}
```

`model-to-world`, `translation`, `rotation`, `scale`, `direction` do not need to be explicitly given. In that case, the camera is at the default position, pointing at (0, 0, -1), and the instance(s) has/have no translation, rotation, and a scale factor of `1`.

The `translation` can also be understood as the `world coordinate`.

You can manually create a scene pretty easily by following this layout, as long as you have the model files and you properly set the `models_folder` variable to be its `absolute` path.

#### Rotation

The rotation is described in `degrees` and converted into `pi radians` internally, but still displayed/exposed as `degrees` to the user. The reason for this is because `degrees` are more intuitive for the user to interpret.

The rotations are defined as counter-clockwise being positive, based on a right-handed coordinate system where z points outward. However, as the camera by default points at the positive z axis, I decided for simplicity to define that the camera rotation parameters (i.e. yaw, pitch, roll) all start at 0. Although depending on the interpretation of the coordinate system this shouldn't be the case, as the z axis is flipped.

Rotation angles are rounded to 3 decimal places. You can change this through the calls to `Utils::round_to`. The following is also the order in which they are applied:

> Yaw: Rotation around the `y` axis
> Pitch: Rotation around the `x` axis
> Roll: Rotation around the `z` axis.

If include both a direction and rotation to the camera, they should logically match, otherwise it will not work properly. If this happens, I chose to override the direction with the rotation parameters.

> `Roll` does not affect the direction vector, so it will not be overridden.

All rotations are restricted to the [0, 2PI] range, if they are smaller or bigger than the range the rotation wraps.
---

You can have as many meshes and instances as you want, and the instances can have any name, but preferably the instance names should be unique.

You are not required to give all of `translation`, `scale` and `rotation`, but at least one of them, though you can have any arbitrary combination. You can, however, give a `model-to-world` matrix which already accounts for all 3 transformations, and ignore the entries.

You can also simply give a `translation` of `0` on all coordinates for every instance and no transformations will be applied to the model.

You can also give camera information, such as its `position`, and a `direction` vector or a `rotation` amount.
i.e.

You can only provide rotations, the direction vectors will be derived from it. You can also provide only direction vectors (camera direction and up vectors), and the rotation parameters (yaw, pitch, and roll) will be derived from them.
If you provide both, the camera direction vectors will be directly set to the same values provided, and the rotation parameters will be set to those provided as well. This means that, in the case that both are provided, they should logically match (i.e. the rotation of the default direction vectors by the respective rotation parameters should end up being the virtually the same as the provided direction vectors).
If none are given, the camera will start in the default position with the default direction vectors (i.e. no translation and the camera points at the center of the positive z-axis. direction: (0, 0, 1), up: (0, 1, 0))

default_direction, default_up, direction, up, default_position, translation
```
"camera": {
	"position": {
		"x": -0.5,
		"y": 0.25,
		"z": -0.1
	},
	"direction": {
		"x": 3,
		"y": 2,
		"z", -2
	}
}
```

or

```
"camera": {
	"position": {
		"x": -0.5,
		"y": 0.25,
		"z": -0.1
	},
	"rotation": {
		"yaw": 3,
		"pitch": 2,
		"roll", -2
	}
}
```


### Quaternion

A `Quaternion` instance holds information about a quaternion. Namely, the `x`, `y`, `z`, and `w` coordinates respectively. As of currently, they are used after the `view space` in the pipeline. 

It is used for -

A `Quaternion` can be instantiated through an empty constructor `Quaternion()`, in which case the `x`, `y`, and `z` coordinates are `0` and `w` is `1`.
Or by calling `Quaternion(x, y, z, w)` with the respective coordinate values.

`AngleAxis`

### Engine

The `Engine` instance holds everything together and controls the flow of the program, by calling the relevant functions through the `main.cpp` file, which is explained in further detail in the [main.cpp](#main-cpp) section.

Its member variables are:
`window`: A pointer to an `SDL_Window` instance.
`renderer`: A pointer to an `SDL_Renderer` instance.
`texture`: A pointer to an `SDL_Texture` instance.
`buffer`: A pointer to the 32-bit `RGB` pixel buffer array.
`title`: Title of the window to be rendered.
`WIDTH`: Width of the window to be rendered.
`HEIGHT`: Height of the window to be rendered.


> The window is resizeable.

## main.cpp

This is where the `main` function is, and where you use the `Engine` instance in order to control the flow of the program.

Here's how it goes in a few steps:
1. Instantiate the `Engine` class, in this case that is the `engine` variable.
2. Run the `Engine` member function `setup`, in order to setup the `Engine`, which initializes `SDL` and allocates a pixel buffer. i.e. `engine.setup()`
3. Set a starting `Scene` to be rendered. You can do so by assigning the `engine.current_scene` variable to a `Scene` instance. The `Scene` constructor variables are explained in [Scene](#scene). i.e. `engine.current_scene = Scene(...)`
4. Run the main execution loop.
	> The framerate is calculated here, but I will ommit these variables for clarity.
	1. The `SDL` events are processed, by calling the `Engine` member function `handle_events`. This is where key presses trigger actions within the program. i.e. engine.handle_events()
	2. Draws the `Scene` instances, if the engine hasn't been paused by pressing the key `P`.
	3. Renders the new frame by calling the `Engine` member function `render`. Through `SDL` routines, it clears the `renderer`, updates the `texture` with the new pixel `buffer`, then updates the screen. i.e. `engine.render()`
	4. If the rendering was too fast, meaning that it took less milliseconds per frame than the given framerate in the should, then the rendering of the next frame is delayed by the difference between the two. You can set the `FPS` in the `Engine` member variable `FPS`. The default value is `60`. The milliseconds per frame is stored in the `Engine` member variable `MSPERFRAME`, and it is calculated using the given `FPS`. i.e. `engine.FPS` and `engine.MSPERFRAME`
	5. The averaged `FPS` over the given `engine.fps_update_interval` (in milliseconds) is printed to the console.
	6. Goes back to the beginning of the loop..

## Events

The events are handled in the `Engine` member function `handle_events`.

These are the available key shortcuts and actions:
`1`: Toggles the rendering of each triangle's lines. It is turned off by default.
`2`: Toggles the rasterization of each triangle. It is turned on by default.
`P`: Pauses/resumes the rendering.
`Keypad -`: Decreases the `FOV` by `1`.
`Keypad +`: Increases the `FOV` by `1`.
`[`: Scales down the `x` and `y` coordinates by the `scale_factor`, which by default is `0.05`.
`]`: Scales up the `x` and `y` coordinates by the `scale_factor`, which by default is `0.05`.
`;`: Decreases the `far` plane `z` coordinate by `0.5`. I hardcoded a limit however, that it can never be less than the `near` plane - `0.5`, so that the `far` plane is not in front of the `near` plane.
`'`: Increases the `far` plane `z` coordinate by `0.5`.
`,`: Decreases the `near` plane `z` coordinate by `0.01`. I also hardcoded a limit that it can never be lower than `0.01`.
`.`: Increases the `near`plane `z` coordinate by `0.01`. There is also a limit that it cannot be greater than the `far` plane.
`J`: Rotates camera around the `y` axis (clockwise/counter-clockwise) (left/right) by `rotation_angle_degrees` which by default is `10`. (Gets converted internally into radians)
`K`: Rotates camera around the `x` axis (clockwise/counter-clockwise) (up/down) by `rotation_angle_degrees` which by default is `10`. (Gets converted internally into radians)
`I`: Rotates camera around the `x` axis (clockwise/counter-clockwise) (up/down) by `rotation_angle_degrees` which by default is `10`. (Gets converted internally into radians)
`L`: Rotates camera around the `y` axis (clockwise/counter-clockwise) (left/right) by `rotation_angle_degrees` which by default is `10`. (Gets converted internally into radians)
`Q`: Rotates the camera around the `z` axis (clockwise/counter-clockwise) by `rotation_angle_degrees` which by default is `10`. (Gets converted internally into radians)
`E`: Rotates the camera around the `z` axis (clockwise/counter-clockwise by `rotation_angle_degrees` which by default is `10`. (Gets converted internally into radians)
`A`: Moves the camera to the left. It decreases the camera's `x` coordinate by `translation_amount`, which by default is `0.01`.
`D`: Moves the camera to the right. It increases the camera's `x` coordinate by `translation_amount`, which by default is `0.01`.
`W`: Moves the camera forward. It increases/decreases the camera's `z` coordinate by `translation_amount`, which by default is `0.01`.
`S`: Moves the camera backward. It decreases/increases the camera's `z` coordinate by `translation_amount`, which by default is `0.01`.

* You can quit by clicking on the `X` button in the GUI.
* You can resize the window by clicking and dragging the edges.

## Utils

The file `Utils.h` contains the implementation of utility functions.

`normalize`: Puts a value within some range into another range. So if you have the number `50` within the range from `0` to `100`, and you want to put it in the range `0` to `1`, the function returns `0.5`.
Example: `Utils::normalize(previous_value, previous_minimum, previous_maximum, new_minimum, new_maximum)`

`round_to`: Rounds the given number to an arbitrary number of decimal places. It simply multiplies the given number by 10 to the given number of decimal places, runs the `cmath` standard library `round` function, and divides back by 10 to the number of decimal places.
Example: `Utils::normalize(3.1415, 2)` returns `3.14`
`Utils::normalize(3.1415, 0)` returns `3`

Currently the only implemented function in this file is `normalize`, which puts a value within some range into another range (i.e. if you have the number `50` in the range `0` to `100` and you want to, in the same scale, leave it in the range `0` to `1`, the function returns the number `0.5`). 
Example: `Utils::normalize(previous_value, previous_minimum, previous_maximum, new_minimum, new_maximum)`

# Implementation details

Follows `Vulkan` conventions.
Uses a right-handed coordinate system.
In `world space` (default):
Positive `x`: right
Positive `y`: up
Positive `z`: backward (for a mesh to be visible it would need to have a negative `z` coordinate in the mesh local space, but during mesh loading it gets flipped to positive. The coordinate system remains the same however, as the rotations, translations, and rasterization are defined relative to it)

In `view space` (default as camera points at (0, 0, 1)):
Positive `x`: left
Positive `y`: up
Positive `z`: forward (nothing logically changes in practice, only semantically)

The clipping plane limits are:
(-1, 1) for the `x` dimension
(-1, 1) for the `y` dimension
(near, far) for the `z` dimension (default is (0.6, 1000))

You can think of the transformation from view space into clip space as rotating the coordinate system 180 degrees counter-clockwise. This makes the Positive `z` axis point forward, and the Positive `y` axis point downward.

The rotations are defined to be from the horizontal axis into the vertical axis?

The drawing order is counter-clockwise.

Near plane defaults to `0.6` but can be any value.
Far plane defaults to `1000` but can be any value.

1. Models are transformed and put into `world space` through the `MODEL_TO_WORLD` matrix.
2. The scene moves relative to the camera and put into `view space` through the `VIEW_MATRIX` matrix.
3. The triangles are geometrically clipped, prior to projection, against all view frustum planes except for the `far` plane: `near`, `top`, `bottom`, `left`, `right`. At first I also clipped against the `far` plane, but since Vulkan and most people just cull any triangle instead of clipping, that's what I chose to do.
4. The triangles get projected and we get into `clip space`, through the `PROJECTION_MATRIX` matrix. Perspective divide. At this point the `z` dimension is in the range (0, 1), and the `w` dimension is the same as the vertex's **old** `z` coordinate, prior to the projection.
5. The `x` and `y` coordinates then get properly converted into `screen space` coordinates (i.e. window coordinates).
6. If drawing the line connections of the vertices (`draw_outline`), the relevant vertex lines are drawn, with the given `outline_color`.
7. If filling (`fill`)/rasterizing the triangles, the relevant triangles are filled/rasterized with the given `fill_color`. This is where the vertices and triangle coordinates are interpolated, and where the fragment shader/texturing/coloring should be implemented.

# Testing

Testing is implemented through `Catch2`.
`TestMain.cpp` sets up and runs the test, it contains the `main` function.
`TestMatrix.cpp` contains matrix related tests.
`TestUtils.cpp` contains utility related tests.

These test files were mostly used for testing/validating the behavior of the matrix operations. It can, and probably should, be expanded to other areas of the code to cover more functionalities.

# Notes

# Features

- Resizing the Window properly resizes the rendered scene.

# Todo

- Use CUDA/ROCm for GPU processing

- Implement element-wise addition and subtraction by scalar values, as can already be done with multiplication and division

- Let user arbitrary un-load or all meshes in order to free memory

- Move matrix related functions such as the identity matrix, rotation, translation, etc... to the matrix files? Or keep it within the engine?

- Improve logging and debugging capabilities

- Implement IMGUI for FPS tracking, loading models/scenes from a file selector, camera settings such as coordinates, speed, fov, near & far plane, edit other settings, modifying instance's coordinates, rotation, include relevant debugging information, etc...

- Account for more edge cases? Such as disallowing user to input negative dimensions when instantiating a matrix? I don't really feel the need to since a "regular" person wouldn't be seeing this.

- Improve exception handling

- Include sub-meshes found in meshes? (i.e. `.obj` files can have multiple different parts that make the mesh, for example if modeling a house, you might have separate meshes for the walls called `Wall_0` or `Floor_2` and so on. Should I create sub-meshes for them? Or just ignore them and only load the rendering information?)

- Optimize and clamp rotation to prevent precision and performance issues

- Change absolute path of the scene folder/files to be relative or either?

- Use the mouse to rotate the camera instead of the keyboard

- Implement lighting/normals

- Implement textures

- Implement shaders

- Implement backface culling for non-visible objects from the camera

- Implement collision/physics

- Test on cross-platform (really just test on Linux) and create a proper cross-platform environment for easy setup

- Isolate components and headers better

- Remove or decrease redundancies

- Broaden the testing

- Optimize and refactor the code in general

- Press 'n' to see normals

- Implement SIMD instructions

- Run a performance profile to find and improve the pipeline bottlenecks

- Add option to attach camera to an instance (i.e. to a character or a scene object), and the camera relative to that object.

- Implement particles and particle simulation (i.e. water and/or waves simulation, sparks, fire, etc...)

- Implement animations

- Implement multi-threading

- Swap division for multiplication where applicable and intuitive, as it is a less expensive operation

- Combine view and perspective projection matrices for less overhead?

