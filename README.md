# Introduction

A 3D software renderer implemented from "scratch", on Windows. More specifically everything that is directly related to rendering other than SDL.

<h3>Libraries used</h3>

| Library      | Version | Usage | URL     |
| :---       |  :----:       |   :----:   |          ---: |
| SDL2        | 2.28.1 | Rendering | https://github.com/libsdl-org/SDL/tree/release-2.28.1 |
| Catch2      |  3.4.0  |  Testing       | https://github.com/catchorg/Catch2/releases/tag/v3.4.0   |
| Nlohmann JSON   |   3.11.2   |   JSON parsing        | https://github.com/nlohmann/json/releases/tag/v3.11.2      |
| ImGUI      |  1.91.9  |  Menus/user interface       | https://github.com/ocornut/imgui/releases/tag/v1.91.9   |
| Nativefiledialog   |   116   |   Windows file dialogs        | https://github.com/mlabbe/nativefiledialog/releases/tag/release_116      |

# Examples


# Details on implementation and how to use

## Matrix

The `Matrix.h` and `Matrix.cpp` files are the header and implementation files for the `Mat` class which represents a matrix.

Internally, the representation of a matrix is that of a 2-dimensional array (`double**`).

The matrix multiplication order is left-to-right. This means that the matrix/vector to be transformed must be to the right of the transformation matrices, and the number of columns in the left `Mat` must match the number of rows in the right `Mat`.
(i.e. if multiplying two vectors, the left vector must be a row vector (1xN), while the right one must be a column vector (Nx1))

The order of operations is: Scaling -> rotation -> translation.
i.e. `world_space_vector = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX * local_space_vector`
> This order is reversed for the view matrix since it represents the opposite transformation of the vertices

You can create a `Mat` instance by calling the initializer function with a 2-dimensional `std::initializer_list`, which is essentially a 2-dimensional array which will hold the rows and columns for the matrix, followed by the row dimension and column dimension.
>Example for a `2x3` matrix: `Mat({{1, 2, 3}, {3, 4, 5}}, 2, 3)`

You can get a value from the matrix by calling the `get` `Mat` member function.
> Example to get the value in the 2nd row and 3rd column: `matrix.get(2, 3)`

You can set a value in the matrix by calling the `set` `Mat` member function.
> Example to set the value in the 2nd row and 3rd column to 5: `matrix.set(5, 2, 3)`

You can transpose a matrix by calling the `transposed` `Mat` member function.
> Example: `matrix.transposed()`

You can (assuming that the dimensions are valid) add `+`, add and assign `+=`, subtract `-`, subtract and assign `-=`, multiply by  `*`, and multiply and assign `*=`.

You can also perform element-wise operations by combining a `scalar` with one of the following operators and a `Mat`, by multiplying `*`, multiplying and assigning `*=`, dividing `/`, and dividing and assigning `/=`.

You can calculate the dot product between vectors by calling the `static` `Mat` function `dot`. This operation is commutative (i.e. order does not matter, neither does whether the vectors are row or column). The only requirements are that both `Mat` must be vectors with the same number of elements (i.e. either the row dimension or column dimension must be `1` while the other dimensions match).
> Example: `Mat::dot(vector_a, vector_b)`

You can calculate the vector norm (magnitude/length) by calling the `Mat` member function `norm`.
> Example: `vec_a.norm()`

You can normalize a vector by calling the `normalize` `Mat` member function. This operation divides the matrix, element-wise, by its norm/magnitude/length. This turns it into a unit vector (norm/magnitude/length of 1).
> Example: `matrix.normalize()`

You can test for equality and inequality, reassign a `Mat` instance to another `Mat` instance through copy & reassignment, or you can also directly reassign it to a new `Mat` instance.

You can print the matrix to the console by calling the `print` `Mat` member function.
> Example: `matrix.print()`

You can also print it to the console using streams. i.e. by calling `std::cout << matrix`.

## Engine
The `Engine` class is a singleton which is responsible for handling the setup, dealing with SDL for direct rendering (pixel wise through buffers), the drawing routines, as well as some math related functions, event handling, storing and managing the window manager, and cleaning everything.

### Triangle
The smallest logical geometric figure is a triangle. A `Triangle` is simply an object that holds 3 vertices, which in this context are 3 `4x1` vectors. (`Mat` instances)

The `4x1` dimension is for, respectively, the `x`, `y`, and `z` 2D/3D space coordinates of the vertex. The `w` 4th dimension is used for storing values before transforms as well as allowing for a 1-step translation through matrix vector multiplication. These are formally called homogeneous coordinates.

There are 2 ways in which you can instantiate a `Triangle`:
1. Calling the constructor `Triangle(vertex_a, vertex_b, vertex_c)`, where `vertex_a` is the `Mat` instantiated vector, and so on for the other vertices.<br>
2. Calling the constructor `Triangles(vertices)`, where `vertices` is an array of `3` vertex (`Mat`) instances (i.e. `const Mat[3] vertices`).

### Quad

The second smallest logical geometric figure is a quad, which is implemented as the struct `Quad`. A `Quad` is a figure which consists of 4 vertices, which can be broken down as 2 triangles (or vice-versa).

There are 4 ways in which you can instantiate a `Quad`:
1. Calling the constructor `Quad(triangle_a, triangle_b)` with both being `Triangle` instances.<br>
2. Calling the constructor `Quad(triangles)` where `triangles` is a `Triangle` array of size `2` (i.e. `const Triangle[2] triangles`).<br>
3. Calling the constructor `Quad(vertex_a, vertex_b, vertex_c, vertex_d)`, where each one is a vertex `Mat` vector.<br>
4. Calling the constructor `Quad(vertices)`, where `vertices` is a `Mat` array of size `4`.

### Mesh

The largest geometric figure is a `Mesh`. It holds information about an arbitrary 3D mesh which can be loaded from an `.obj` file or hardcoded (by, for example, manually defining the vertex array and the face indices).
A face can be made up of either `3` or `4` vertices, so both triangles and quads are considered faces.

Its member variables are the following:<br><br>
`vertices`: An `std::vector` of type `Mat`, which stores all of the figure's vertices.<br><br>
`faces_indices`: An `std::vector` of type `std::vector<uint32_t>>`, which stores all the face indices.  The reason why there is a nested `std::vector` here is because each individual face is its own `std::vector<uint32_t>` (an array of unsigned integers), which identify the respective vertex by indexing from the `vertices` array.<br><br>
`normals`: The same behavior described above for `vertices`, except that this is meant for normals.<br><br>
`normal_indices`: The same behavior described above for `faces_indices`, except that this is meant for normals.<br><br>
`mesh_filename`: Stores the filename of mesh.<br><br>
`mesh_id`: Uniquely identifies the mesh. Throughout the program, this value is set as the number of total meshes loaded for the current scene `total_ever_meshes`, which then gets incremented by `1`.<br><br>

> **The face indices start at `1`.**<br>
> **The z coordinate for the vertices and normals are flipped for the sake of intuition, as otherwise the world z coordinate would be negative for vertices in front of the camera.**<br>


> The `obj` formatted meshes must be within the given `models_folder`, and `mesh_filename` must be a valid file within that folder.<br>
> The `models_folder` must end with a `\`

You are able to get the number of vertices from a `Mesh` by calling the member function `total_vertices`, or `total_faces` in the case of faces.

There are 2 ways in which you should instantiate a `Mesh`:

1. Calling the constructor `Mesh()`, which instantiates a hardcoded cube
	> You can find specific values, coordinates, and other values such as the `width`, `height`, and `depth` of the cube within the function definition.<br><br>
	> The `mesh_filename` will be set to `cube.obj`.
2. Calling the constructor `Mesh(current_scene.total_ever_meshes)`, which simply creates and empty `Mesh` with the given ID and subsequently increments it
3. Calling the constructor `Mesh(model_path, mesh_filename, current_scene.total_ever_meshes)`, which loads the `obj` file `mesh_filename` in the folder `model_path`
	> `model_path` is the relative or absolute path to the `obj` formatted file<br>
	> `mesh_filename` is the actual mesh's filename, including the extension<br>
	
### Instance

An instance is pretty self-explanatory. It is simply an `Instance` of some loaded `Mesh`. This is done so that each rendered object in the scene gets its own parameters independent of any other while also discarding the need to duplicate the same mesh's data over and over when placing the same mesh in distinct settings within the scene.

Its member variables are the following:<br><br>
`instance_name`: You can give a name to an instance in order to help you identify it. If no instance name is given, the `Instance` member function `create_instance_nameid` is used in order to create a name for it, which is preceded by its mesh's filename. The instance name can be set through the instances menu or programmatically by calling `instance.create_instance_nameid(total_ever_instances)` or initializing through a constructor with the given instance name.<br><br>
`instance_id`: Uniquely identifies the `Instance`. Behaves the same way as `mesh_id`, except that it is used for instances.<br>
`mesh`: A pointer to the `Mesh` which will be rendered.<br>
`tx`: Translation along the `x` axis (positive values move to the left since it is a z-flipped right-handed coordinate system)<br>
`ty`: Translation along the `y` axis<br>
`tz`: Translation along the `z` axis<br>
`yaw`: Rotation around the `y` axis<br>
`pitch`: Rotation around the `x` axis<br>
`roll`: Rotation around the `z` axis<br>
`orientation`: A [Quaternion](#Quaternion) which represents the instance's orientation<br>
`sx`: Scaling along the `x` axis<br>
`sy`: Scaling along the `y` axis<br>
`sz`: Scaling along the `z` axis<br>
`TRANSLATION_MATRIX`: A `4x4` translation matrix which holds an arbitrary translation<br>
`ROTATION_MATRIX`: A `4x4` rotation matrix which holds arbitrary rotation(s)<br>
`SCALING_MATRIX`: A `4x4` scaling matrix which holds an arbitrary scale<br>
`MODEL_TO_WORLD`: A `4x4` matrix which will hold the final transformation matrix for the given instance<br>
`show`: A `bool` toggle for whether the `Instance` will be rendered or not.
`is_light_source`: Positive when the given instance belongs to a light source, negative otherwise<br>
`is_axes`: Whether the given instance is the transform axes<br>
`has_axes`: Whether the given instance has the transformed axes attached to it<br>

There are so many ways in which you can instantiate an `Instance` (and they are not that much different) that it would be better if you look at the `Instance.h` header yourself. In summary, they revolve around different combinations of the instance's name, `Mesh`, translation, rotation, scaling parameters, and/or their respective transformation matrices.
> One thing that should almost always should be present in the constructor is the `current_scene.total_ever_instances` in order to create a unique ID for that instance.

### Scene
> The `Scene` configuration file must be in a valid `json` format and within the given `scene_folder`. The `scene_filename` must be a valid file within the folder as well.<br><br>
> The `scene_filename` must include the file extension.<br><br>
> The `scenes_folder` must have a `/` at the end if preferably an `absolute` path. Otherwise you need to figure out the correct file tree for relative indexing.
> The `scenes_folder` can be seen and updated in the `Scene` menu, as well as the scene name for loading and saving.

A `Scene` instance holds information about the scene to be rendered. This includes the meshes loaded for the scene, as well as the instances and other relevant information.

You can get a copy of a `Mesh` by calling `scene.get_mesh(mesh_id, mesh)`, where `scene` is the variable that instantiates your `Scene`, `mesh_id` is the id of the `Mesh` you want the copy of and `mesh` is the variable which will store the resulting copy if the retrieval is successful.
The same goes for an `Instance`, except that the function and parameters are related to the instance - `scene.get_instance(instance_id, instance)`.

> These functions returns `true` if successful, `false` otherwise.

You can get a `pointer` to a `Mesh` by calling `scene.get_mesh_ptr(mesh_id)`. It works similarly to the above functions, so you can use the respective getter function to the type you need. In this case no reference needs to be passed as an argument, since no copies are made.

> The requested pointer is returned in case of success, or a `nullptr` is returned in the case of failure.

You can also use the names instead of the ids, though there may be a conflict if there are somehow other meshes/instances with the same name.

Its member variables are:<br><br>
`total_ever_meshes`: Keeps track of the number of loaded meshes. (This is an ever increasing integer which is used for the mesh ids)<br>
`total_meshes`: Same as `total_ever_meshes`. Currently obsolete.<br>
`total_ever_instances`: Keeps track of the number of instances loaded since the scene has been loaded. (This is an ever increasing integer which is used for the instance ids)
`total_instances`: Keeps track of the number of instances currently in the scene.<br>
`total_triangles`: Keeps track of the number of triangles.<br>
`total_vertices`: Keeps track of the number of vertices.<br>
`rendered_meshes`: Keeps track of the number of rendered meshes<br>
`rendered_instances`: Keeps track of the number of rendered instances<br>
`rendered_triangles`: Keeps track of the number of rendered triangles<br>
`rendered_vertices`: Keeps track of the number of rendered vertices<br>
> These variables are mostly obsolete, not properly updated, and currently unused (with the exception of `total_ever_instances` and `total_ever_meshes`)

`scene_meshes`: An `std::deque` of type `Mesh` which holds the scene `Mesh` instances.<br>
`scene_instances`: An `std::deque` of type `Instance` which holds the scene `Instace` instances.<br>
`scene_name`: Stores the name of the `Scene`. As of right know is left unused, but could eventually be useful for identifying individual scenes.<br>
`scene_filepath`: Stores the filepath of the `json` formatted `Scene` configuration file .<br>
`scene_data`: The scene `JSON` data will be stored here. This JSON parser is nlohmann's `nlohmann::json`, which is an external library.

The `Scene` instances can be saved to and loaded from configuration files in `json` format. This can be done through the `Scene` tab in the menus or programmatically through the `Scene` member functions `save_scene` and `load_scene`, respectively.

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

A default scene is load_default_scene()
Also cube scene is Scene()

This is how they are formatted:

**scene.json**
```
{
	"scene": "default_scene",
	"camera": {
		"translation": {
			"x": 0,
			"y": 0,
			"z": 0
		},
		"direction": {
			"x": 0,
			"y": 0,
			"z": 1
		},
		"up": {
			"x": 0,
			"y": 1,
			"z": 0
		}
	},
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

### Camera

### Quaternion

#### Rotation

The rotation is described in `degrees` and converted into `pi radians` internally, but still displayed/exposed as `degrees` to the user. The reason for this is because `degrees` are more intuitive for the user to interpret.

The rotations are defined as counter-clockwise being positive, based on a right-handed coordinate system where z points outward. However, as the camera by default points at the positive z axis, I decided for simplicity to define that the camera rotation parameters (i.e. yaw, pitch, roll) all start at 0. Although depending on the interpretation of the coordinate system this shouldn't be the case, as the z axis is flipped.

Rotation angles are rounded to 3 decimal places. You can change this through the calls to `Utils::round_to`. The following is also the order in which they are applied:

> Yaw: Rotation around the `y` axis
> Pitch: Rotation around the `x` axis
> Roll: Rotation around the `z` axis.

If including both a direction and rotation to the camera, they should logically match. When this is the case, I chose to give precedence for the direction vector, meaning that even though the rotation parameters are given, the rotation parameters will be derived from the direction vectors, so that both match regardless. So they may differ slightly even if they're correct.

> `Roll` does not affect the direction vector, so it will not be overridden.

All rotations are restricted to the [0, 2PI] range, if they are smaller or bigger than the range the rotation wraps.
---

You can have as many meshes and instances as you want, and the instances can have any name, but preferably the instance names should be unique.

You are not required to give all of `translation`, `scale` and `rotation`, but at least one of them, though you can have any arbitrary combination. You can, however, give a `model-to-world` matrix which already accounts for all 3 transformations, and ignore the entries.

You can also simply give a `translation` of `0` on all coordinates for every instance and no transformations will be applied to the model.

You can also give camera information, such as its `position`, and a `direction` vector or a `rotation` amount.
i.e.

You can only provide either rotations (yaw, pitch, roll) or direction vectors (direction, up), or both. If only one was provided, the other one gets derived at runtime, from the one that was given.
If you provide both, the rotation parameters get derived at runtime from the given direction vectors, so the given rotation parameters are not directly set. This means that, in the case that both are provided, they should be logically equivalent (i.e. the rotation of the default direction vectors by the respective rotation parameters should end up being virtually the same as the provided direction vectors).
If none are given, the camera will start in the default position with the default direction vectors (i.e. no translation and the camera points at the center of the positive z-axis. direction: (0, 0, 1), up: (0, 1, 0))

default_direction, default_up, direction, up, default_position, translation
```
"camera": {
	"translation": {
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
	"translation": {
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

### Light

....

### Windows/tabs

## Events

The events are handled in the `Engine` member function `handle_events`.

These are the available key shortcuts and actions:
`1`: Toggles the rendering of each triangle's lines. It is turned off by default.
`2`: Toggles the rasterization of each triangle. It is turned off by default.
`3`: Toggles the shading of each triangle. It is turned on by default.
`4`: Toggles the backface culling of each triangle. It is turned on by default.
`5`: Toggles the depth testing of each pixel. It is turned on by default.
`P`: Pauses/resumes the rendering.
`T`: Prints information about the camera. Namely the camera position, direction and up vectors, rotation parameters.
`G`: Saves the current scene metadata to a file called `tst.json` that is saved in the `models` folder. Can be modified in the variable `scene_save_name` @ `Engine.h`.
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
`E`: Rotates the camera around the `z` axis (clockwise/counter-clockwise) by `rotation_angle_degrees` which by default is `10`. (Gets converted internally into radians)
`A`: Moves the camera to the left. It decreases the camera's `x` coordinate by `translation_amount`, which by default is `0.01`.
`D`: Moves the camera to the right. It increases the camera's `x` coordinate by `translation_amount`, which by default is `0.01`.
`W`: Moves the camera forward. It increases/decreases the camera's `z` coordinate by `translation_amount`, which by default is `0.01`.
`S`: Moves the camera backward. It decreases/increases the camera's `z` coordinate by `translation_amount`, which by default is `0.01`.

* You can quit by clicking on the `X` button in the GUI.
* You can resize the window by clicking and dragging the edges.

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
>
> 

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

The renderer uses 4 coordinate systems, of which only the last one is different.

The first 3 coordinate systems represent 3 dimensional space, are all right-handed. Here, the positive x-axis points to the right, the positive y-axis points up, and the positive z-axis points outward ("out of the screen/toward the viewer").

Firstly there is `local space`, which is essentially a 3D model's local coordinate system before they are added to the scene. If you have worked with modelling software like Blender for example, you can think of everything within the Blender scene if you were to export it as being in `local space`.

Secondly there is `world space`, which, as the name implies, is the space which holds all the 3D models present in the scene, in absolute values. Each model present in the scene has a transformation matrix which transforms all the vertices for that `Instance`'s 3D mesh from `local space` to `world space`, essentially moving it to the scene including the relevant transformations.<br>
The matrix responsible for this conversion is called `MODEL_TO_WORLD`, and every `Instance` has it. This matrix is a combination of 3 other matrices, first the `SCALING_MATRIX`, secondly the `ROTATION_MATRIX`, then finally the `TRANSLATION_MATRIX`. These matrices are built by the individual values for each `Instance` and finally combined to make the `MODEL_TO_WORLD` matrix.

> In a right-handed coordinate system the `z` axis points "out of the screen/toward the viewer", this means that for a mesh to be visible it would need to have a negative `z` coordinate in world space. However, during mesh loading I purposefully flip it so that a positive `z` value means "farther", which is more intuitive but less semantically ideal. The coordinate system remains the same however.

Thirdly there is `view space` or `camera space`, as it represents a "view" into the world from the camera's point of view. It essentially contains all transformations relative to the camera. This transformation is stored in the `VIEW_MATRIX`. It can also be seen as a transformation that is the inverse of the camera transform, because, relative to the camera, whenever the it moves to the right for example, the object in front of it appears to move to the left, and this means that whatever happens to a logical camera can be simulated by performing the opposite operations to each vertex. The `VIEW_MATRIX` is constructed from a position and a direction through the `LookAt` function, and it encapsulates the translation and rotation akin to what happens above for instances in `world space`.

After this, geometric clipping is performed for each triangle against each of the camera's 6 planes in `view space` (Near, far, left, right, top, bottom planes).

Now perspective projection is applied to the vertices. At this stage the coordinate space is considered to be in `clip space`.
> Note that the `z` transformation here is non-linear.

Finally, for the 3-dimensional space, perspective division is performed over all vertices by their `w` (which is simply their `z` coordinate prior to the projection).
We are now at what is called `NDC (Normalized Device Coordinates) space`, where, theoretically, every point/vertex that made it to this stage is viewable through the camera. This space's boundaries are in the ranges [-1, 1] for the x-axis, [-1, 1] for the y-axis, and [0, 1] for the z-axis.

Here, a vertex whose `z` coordinate is the same as the `near` plane value would have a `z` of 0, and if I were to be the same as the `far` plane, it would have a `z` of 1.


We can then start processing the vertices. Here `backface culling` is performed, as well as some `light` shading, if `Flat` shading it is all performed here. If `Gouraud` shading the vertex colors are calculated here for each vertex (they get interpolated at the pixel level later). If `Phong` shading, the vertex normals are calculated for each vertex (these also get interpolated later).

The last coordinate system is `screen space` or `framebuffer space`. Here the vertices' `x` and `y` coordinates are flipped and scaled by the camera's `SCALE_MATRIX`. The `y` axis is flipped because the `y` coordinate increases as you go down when it comes to pixels, but in 3D space it increased as it went up. As for the `x` axis, since I flipped the `z` coordinates when loading the meshes, the resulting logically and semantically correct right-handed coordinate system with this configuration has the positive `x` axis to the left (not to the right), which means that going to the right would instead decrease the `x` coordinate for the pixel. Now we are finally in the 2-dimensional space which represents the window coordinates.

The drawing order is counter-clockwise.

Near plane defaults to `0.01` but can be any value.
Far plane defaults to `1000` but can be any value.

If drawing the line connections of the vertices (`draw_outline`), the relevant lines are drawn, with the given `outline_color`.
If filling (`fill`)/rasterizing the triangles, the relevant triangles are filled/rasterized with the given `fill_color`. This is where the vertices and triangle coordinates are interpolated, and where the fragment shader/texturing/coloring should be implemented.

# Testing

Testing is implemented through `Catch2`.
`TestMain.cpp` sets up and runs the test, it contains the `main` function.
`TestMatrix.cpp` contains matrix related tests.
`TestUtils.cpp` contains utility related tests.

These test files were mostly used for testing/validating the behavior of the matrix operations. It can, and probably should, be expanded to other areas of the code to cover more functionalities.

# Notes

# Features

- Window is resizeable and the renderer is updated accordingly

# Todo

- Use CUDA/ROCm for GPU processing

- Implement element-wise addition and subtraction by scalar values, as can already be done with multiplication and division

- Let user load meshes with the same name

- Let user delete meshes

- Improve logging and debugging capabilities

- Improve exception handling

- Include sub-meshes found in meshes (`.obj` files can have multiple different parts that make the mesh, for example if modeling a house, you might have separate meshes for the walls called `Wall_0` or `Floor_2` and so on. Should I create sub-meshes for them? Or just ignore them and only load the rendering information?)

- Fix spotlight light type behavior

- Implement textures

- Implement shaders

- Implement backface culling for non-visible objects from the camera

- Implement collision/physics

- Make sure it works cross-platform and create a proper environment for easy setup

- Remove or decrease redundancies

- Broader testing coverage

- Optimize and refactor the code in general

- Allow user to see the normals (similar to how it's done with the transform axes)

- Implement SIMD instructions

- Run a performance profile to find and improve the pipeline bottlenecks

- Add option to attach camera to an instance (i.e. to a character or a scene object)

- Implement particles and particle simulation (i.e. water and/or waves simulation, sparks, fire, etc...)

- Implement animations

- Implement multi-threading

- Swap division for multiplication where applicable and intuitive, as it is a less expensive operation

- Combine view and perspective projection matrices for less overhead and wherever else possible

- Include and allow user to load/save quaternion into/from scene configuration file

- Write a helper function to decompose matrices (i.e. rotation matrix -> yaw, pitch, roll or translation matrix -> tx, ty, tz or view matrix -> everything, etc...)

- Account for non-default default values (i.e. what would be considered the absolute 0) for vectors and other values, and describe their utility.

- Unflip the z when loading the meshes an instead set the default camera direction to (0, 0, -1) instead of flipping the z during mesh loading and the default camera direction to (0, 0, 1). This would be more semantically accurate to the coordinate system and possibly avoid confusion if manipulating vertices, but also less intuitive for the users.

- Describe, in this README, the keys for rotating some given mesh instance at runtime

- Fix the lighting implementation as it is not properly implemented currently. Some triangles that shouldn't be visible are, and looking in the opposite camera direction makes models invisible.

- Check that deriving the yaw, pitch, and roll properly work by calling the function `Engine::Euler_GetAnglesFromDirection`

- Properly handle the edge case for when vectors are antiparallel (point in opposite directions) within the 3D cross product function 