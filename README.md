# Introduction

A 3D software renderer implemented from "scratch", on Windows. More specifically everything that is directly related to rendering other than SDL.

**Make sure to update the scene folder and models folder in the menu to where you downloaded or intend to store scenes and models. You can also change the default values in `SceneTab.h` and recompile.**

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
> You can also print it to the console using streams. i.e. by calling `std::cout << matrix`.

You can get an identity matrix of an arbitrary dimensionn by calling the `identity_matrix` `Mat` static function, which returns a square matrix of NxN dimension, where `N` is the given dimension.
> Example: `Matrix::identity_matrix(4)`

You can get a translation matrix with `tx` translation in the x axis, `ty` in the y axis, `tz` in the z axis by calling the `translation_matrix` `Mat` static function. The result is always a 4x4 translation matrix.
> Example: `Matrix::translation_matrix(tx, ty, tz)`

Similarly, you can get a scale matrix with `sx` scaling in the x axis, `sy` in the y axis, `sz` in the z axis, by calling the `scale_matrix` `Mat` static function. The result is also always a 4x4 scaling matrix.
> Example: `Matrix::scale_matrix(sx, sy, sz)`

You can get the 2-dimensional cross product between two vectors by calling the `CrossProduct2D` static `Mat` function with both vectors as parameters. This function returns a `double`.
> Example: `Matrix::CrossProduct2D(v1, v2)`

Similarly, you can get the 3-dimensional cross product by calling the `CrossProduct3D` static `Mat` function. This function returns a 4x1 `Mat` (vector).
> Example: `Matrix:CrossProduct3D(v1, v2)`

## Engine
The `Engine` class is a singleton which is responsible for handling the setup, SDL for direct rendering (pixel wise through buffers), the drawing routines, as well as some math related functions, event handling, storing and managing the window manager, and cleaning everything. It holds everything together and controls the flow of the program, which is explained further in the [main.cpp](#main-cpp) section.

Its member variables are:
`window`: A pointer to an `SDL_Window` instance.
`renderer`: A pointer to an `SDL_Renderer` instance.
`texture`: A pointer to an `SDL_Texture` instance.
`event`: An `SDL_Event` instance.
`buffer`: A pointer to the 32-bit `RGB` pixel buffer array.
`CLEAR_COLOR`: The `SDL_RenderClear` color.
`title`: Title of the window to be rendered.
`WIDTH`: Width of the window to be rendered.
`HEIGHT`: Height of the window to be rendered.
`window_manager`: The window manager.

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
> The instances can have any name, but preferably they should be unique.

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

`default_world_up`: The vector that represents the default world up axis.
`default_world_right`: The vector that represents the default world right axis.
`default_world_forward`: The vector that represents the default world forward axis.

`camera`: An instance of the [Camera](#Camera).<br>
`light_source`: The scene's light source, which is represented by a [Light](#Light) instance. As of right now, there can only be a single light source in the scene, which is managed through this variable, regardless of whether lighting is enabled for the scene.

`axes_mesh`: The [Mesh](#Mesh) which represents the transform axes.<br>
`axes_instance`: The [Instance](#Instance) which represents the transform axes.
> The [Mesh](#Mesh) that is loaded for this model is the `axes.obj` file. It should be within the models folder.<br>
> This object is a 3D axes that gets rendered at the same position and orientation as the select model in the scene, so it serves as a reference point to which scene object is selected as well as for the transformations and orientation.

`BG_COLOR`: An hexadecimal value representing the scene's background color.
`LINE_COLOR`: An hexadecimal value representing the scene's line drawing color.
`FILL_COLOR`: An hexadecimal value representing the scene's triangle fill/rasterization color.

The `Scene` instances can be saved to and loaded from configuration files in `json` format. This can be done through the `Scene` tab in the menus or programmatically through the `Scene` member functions `save_scene` and `load_scene`, respectively.

The `Scene` can be instantiated in 4 ways:

1. Calling the empty constructor `Scene()`. In this case the default values are set and the transform axes and light source's mesh/instances would need to be manually loaded.<br><br>
2. Calling the constructor by passing the models folder `Scene(models_folder)`. This creates an empty scene with the transform axes and the light source properly loaded and setup.<br><br>
3. Calling the constructor by passing the models folder, rotation orientation, and whether to enable verbose logging (i.e. `Scene(models_folder, rotation_orientation, verbose`). This performs the transform axes and light source setup and additionally adds a cube (the `cube.obj` mesh in the models folder) to the scene, as well as an arrow (`arrow.obj`) for the light source.<br><br>
4. Calling the constructor by passing the `scene_folder`, `scene_filename`, `models_folder`, `rotation_orientation`, `update_camera_settings`, and `verbose`. This loads the scene from the file `scene_filename` within the folder `scene_folder`, with the models loaded from the folder `models_folder` in the given `rotation_orientation`, using the camera settings given in the scene configuration file if `update_camera_settings` is `true` (resets it to default otherwise), and logs to the console some information if `verbose` is `true`.

A given `Scene` instance can be saved by calling the `save` member function, by passing the folder in which to save it and its filename as arguments.<br>
> Example: `scene.save("D:\\Scenes\\", "saved_scene.json")`

As it stands, a blank `Scene` is created on program startup. It is up to the user to load any given `Scene` through the menu, or edit the blank scene. An error message warning you will be displayed if your scene file or the models within your scene could not be loaded.
> There are 3 types of errors handled when attempting to load a scene:
> 1. `SceneError::ModelLoad`: Occurs when the model couldn't be loaded. It can be due to an invalid format, or more likely due to an incorrect filepath. Can usually be fixed by correctly setting the model folder and the respective model file within it.
> 2. `SceneError::SceneLoad`: Occurs when the scene couldn't be loaded due to an invalid filepath. Can usually be fixed by correctly setting the scene folder and the respective scene file within it.
> 3. `SceneError::JSONParsing`: Occurs when the scene couldn't be loaded due to an invalid JSON format when parsing. This is usually caused by extra or missing commas, braces, brackets, quotation marks, etc... Make sure that the JSON is valid.

`errored_path`: Holds an string with the path to the file (model or scene) which was attempted to be loaded when an error occurred (empty otherwise).

This is an example of how JSON scene configuration files are formatted (this file can be found within the `scenes` folder in this repo):

**cube.json**
```
{
    "scene": "cube.json",
    "camera": {
        "default_position": {
            "x": 0.0,
            "y": 0.0,
            "z": 0.0
        },
        "translation": {
            "x": 0.0,
            "y": 0.0,
            "z": 0.0
        },
        "default_direction": {
            "x": 0.0,
            "y": 0.0,
            "z": 1.0
        },
        "direction": {
            "x": 0.0,
            "y": 0.0,
            "z": 1.0
        },
        "default_up": {
            "x": 0.0,
            "y": 1.0,
            "z": 0.0
        },
        "up": {
            "x": 0.0,
            "y": 1.0,
            "z": 0.0
        },
        "rotation": {
            "x": 0.0,
            "y": 0.0,
            "z": 0.0
        }
    },
    "bg_color": {
        "r": 0,
        "g": 0,
        "b": 0
    },
    "line_color": {
        "r": 0,
        "g": 255,
        "b": 0
    },
    "fill_color": {
        "r": 102,
        "g": 40,
        "b": 92
    },
    "light": {
        "type": "directional",
        "enabled": true,
        "color": {
            "r": 102,
            "g": 40,
            "b": 92
        },
        "intensity": 1.0,
        "minimum_exposure": 0.1,
        "model": "arrow.obj",
        "position": {
            "x": 0.07,
            "y": 0.15,
            "z": 0.47000000000000003
        },
        "rotation": {
            "x": -28.82203865457232,
            "y": 21.741555656980953,
            "z": 109.75987370009548
        },
        "direction": {
            "x": 0.3245333323392281,
            "y": 0.4820907072649036,
            "z": 0.8137976813493758
        },
        "up": {
            "x": -0.8137976813493751,
            "y": -0.2961981327260267,
            "z": 0.4999999999999952
        },
        "scale": {
            "x": 1.0,
            "y": 1.0,
            "z": 1.0
        }
    },
    "models": {
        "cube.obj": {
            "instances": {
                "1": {
                    "show": true,
                    "translation": {
                        "x": 0.0,
                        "y": 0.0,
                        "z": 1.0
                    },
                    "rotation": {
                        "y": 0.0,
                        "x": 0.0,
                        "z": 0.0
                    },
                    "scale": {
                        "x": 1.0,
                        "y": 1.0,
                        "z": 1.0
                    },
                    "model_to_world": [
                        [
                            1.0,
                            0.0,
                            0.0,
                            0.0
                        ],
                        [
                            0.0,
                            1.0,
                            0.0,
                            0.0
                        ],
                        [
                            0.0,
                            0.0,
                            1.0,
                            1.0
                        ],
                        [
                            0.0,
                            0.0,
                            0.0,
                            1.0
                        ]
                    ]
                }
            }
        }
    }
}
```

Virtually all of these parameters can be ommitted, in which case the values will be set to default. When it comes to the camera and light source, you can provide only the direction vectors, only the rotation parameters (yaw, pitch, and roll), or both.
> The rotation parameters (yaw, pitch, roll) are described in degrees in the configuration file, but are converted to radians internally

> Keep in mind that the rotation parameters and direction vectors should match, otherwise there will be a conflict
> The vectors do not need to be of unit length and the rotation parameters do not need to be within an specific range.

The values preceded by `default_` here are better left unchanged. My idea was that the user could have an arbitrary starting point from the world space's origin and/or orientation, and the values would be infered accordingly, I haven't had the time to test this out properly nor implement it as I wanted to due to the time constraints I have and a feature of little relevance that virtually nobody would ever touch, so it should not work as intended currently, although some of its foundation and behavior has been implemented.

The `scene` field is meant to be the name of the scene (not filename), but since no functionality that uses this was implemented it is obsolete.

The camera settings are described in [Camera](#Camera).
The light settings are described in [Light](#Light).
The instance settings are described in [Instance](#Instance)

The `bg_color` setting controls the color of the background.
The `line_color` setting controls the color of the lines drawn.
The `fill_color` setting controls the color of the triangles/rasterization. (Irrespective of lighting. This is also called ambient light)

You can manually create a scene by following this layout, as long as you have the model files and you properly set the models folder. Though it would evidently be tiresome for a large scene.

#### Camera
You can also give camera information, such as its `position`, and a `direction` vector or a `rotation` amount.

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

`camera_position`: A `4x1` vector `Mat` instance that has the position of the camera in `world space`. The first 3 components are the `x`, `y`, and `z` coordinates, respectively. The 4th dimension is always `1` and solely used to simplify matrix operations.<br>
`camera_direction`: A `4x1` vector `Mat` instance that has the direction (vector) of where the camera is pointing toward. The first 3 components are the `x`, `y`, and `z` coordinates, respectively. The 4th dimension is always `1` and solely used to simplify matrix operations.<br>
`camera_yaw`: A `double` representing the camera `yaw` in `world space` from the default orientation.<br>
`camera_pitch`: A `double` representing the camera `pitch` in `world space` from the default orientation.<br>
`camera_roll`: A `double` representing the camera `roll` in `world space` from the default orientation.<br>
`VIEW_MATRIX`: A `4x4` matrix `Mat` instance that represents the `view` matrix, which is responsible for dealing with the camera related transformations/movement.<br>

#### Light



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

### Rotation
> The rotation parameters (yaw, pitch, roll) are described in degrees in the configuration file, but are converted to radians internally. The reason for this is because degrees are more intuitive for the user interpretation.

The rotations are defined as positive when counter-clockwise, based on a right-handed coordinate system where the positive z axis points outward. However, as the camera by default points at the positive z axis, I decided to define that the camera rotation parameters (i.e. yaw, pitch, roll) all start at 0, as if the camera were pointing at (0, 0, -1) instead of (0, 0, 1). Depending on the interpretation of the coordinate system this shouldn't be the case, as I purposefully flipped the z axis during mesh loading for better intuition, as now positive z coordinates are in front of the camera.

The rotation order applied is yaw first, then pitch, then roll.

> Yaw: Rotation around the `y` axis
> Pitch: Rotation around the `x` axis
> Roll: Rotation around the `z` axis.

If including both a direction and rotation in the scene file, they should logically match.

### Quaternion

A `Quaternion` instance holds information about a quaternion. Namely, the `x`, `y`, `z`, and `w` coordinates respectively. As of right now, they are used after the `view space` in the pipeline and for retrieving the rotation axis and figuring out `yaw`, `pitch`, and `roll` from direction vectors through its `GetAngles` and `GetRoll` member functions. 

A `Quaternion` can be instantiated through an empty constructor `Quaternion()`, in which case the `x`, `y`, and `z` coordinates are `0` and `w` is `1`.
Or by calling `Quaternion(x, y, z, w)` with the respective coordinate values.

`AngleAxis`


### Windows/tabs

## Events

The events are handled in the function `handle_events`, which is an `Engine` member function. It is implemented separately in the file `Events.cpp`.

Here are the implemented key events:
`1`: Toggles the ImGUI menu.
`W`: Moves the camera forward
`S`: Moves the camera backward
`A`: Moves the camera left
`D`: Moves the camera right
`Up arrow`: Moves the camera up
`Down arrow`: Moves the camera down

Mouse motion is also caught here for rotating the camera with respect to mouse movement.

You can resize the window by clicking and dragging the edges.

You can quit by clicking on the `X` button in the GUI.

## main.cpp

This is where the `main` function is, and where you use the `Engine` instance in order to control the flow of the program.

Here's how it goes in a few steps:
1. Instantiate the `Engine` class, in this case that is the `engine` variable.
2. Run the `Engine` member function `setup`, in order to setup the `Engine`, which initializes `SDL` and allocates a pixel buffer.
3. Set a starting `Scene` to be rendered. You can do so by assigning the `engine.current_scene` variable to a `Scene` instance. The `Scene` constructor variables are explained in [Scene](#scene). i.e. `engine.current_scene = Scene(...)`
4. Run the main execution loop.
	1. The `SDL` and `ImGUI` events are processed, by calling the `Engine` member function `handle_events`.
	2. The `Scene` instances (and optionally the transform axes) are drawn to the pixel and depth buffers.
	3. Starts the `ImGUI` frame, draws some text, and draws the `ImGUI` windows if they are toggled by pressing `1`.
	4. Renders the new frame by calling the `Engine` member function `render`. Through `SDL` routines, it clears the `renderer`, updates the `texture` with the new pixel buffer, then updates the screen.
	5. If the rendering was too fast, meaning that it took less milliseconds per frame than the given framerate limit should, then the rendering of the next frame is delayed by the difference between the two values.
	> You can update these values in the [Settings tab](#settings-tab) in the mnu or in the `SettingsTab.h` file then recompiling.
	6. The averaged fps over the given update interval (in milliseconds) is updated, then displayed and printed to the console.
	7. Goes back to the beginning of the loop...



## Utils

The file `Utils.h` contains the implementation of some utility functions.

`normalize`: Converts a value within some range to some other range. So if you have the number `50` within the range from `0` to `100`, and you want in the range `0` to `1`, the function returns `0.5`. The first argument to the function is the previous value which will be converted, the previous minimum value in that range, the previous maximum value in that range, the new minimum value for the new range, and the new maximum value.
> Example: `Utils::normalize(50, 0, 100, 0, 1)` returns `0.5`.

`round_to`: Rounds the given number to an arbitrary number of decimal places. It simply multiplies the given number by 10 to the given number of decimal places, runs the `cmath` standard library `round` function, and divides back by 10 to the number of decimal places.
> Example: `Utils::normalize(3.1415, 2)` returns `3.14`
> `Utils::normalize(3.1415, 0)` returns `3`

`clamp`: Clips a value to a given range. If it is smaller than the minimum value in the range, it gets set to that value, and vice-versa if it is higher than the maximum value. Otherwise it doesn't change.
> Example: `Utils::clamp(-2, 1, 100)` returns `1`

# Implementation details

The renderer uses 4 coordinate systems, of which only the last one is different.

The first 3 coordinate systems represent 3 dimensional space, and are all right-handed. Here, the positive x-axis points to the right, the positive y-axis points up, and the positive z-axis points outward (out of the screen/toward the viewer).

Firstly there is `local space`, which is essentially a 3D model's local coordinate system before they are added to the scene. If you have worked with modelling software like Blender for example, you can think of everything within the Blender scene as being in `local space`.

Secondly there is `world space`, which, as the name implies, is the space which holds all the 3D models present in the scene, in absolute values. Each model present in the scene has a transformation matrix which transforms all the vertices for that `Instance`'s 3D mesh from `local space` to `world space`, essentially moving it to the scene including the relevant transformations.<br>
The matrix responsible for this conversion is called `MODEL_TO_WORLD` and every `Instance` has it. This matrix is a combination of 3 other matrices, first the `SCALING_MATRIX`, secondly the `ROTATION_MATRIX`, then finally the `TRANSLATION_MATRIX`. These matrices are built by the individual values for each `Instance` and finally combined to make the `MODEL_TO_WORLD` matrix.

> In a right-handed coordinate system the `z` axis points "out of the screen/toward the viewer", this means that for a mesh to be visible it would need to have a negative `z` coordinate in world space. However, during mesh loading I purposefully flip it so that a positive `z` value means "farther", which is more intuitive but less semantically sound. The coordinate system remains the same however.

Thirdly, there is `view space` or `camera space`. It represents a "view" into the world from the camera's point of view. It essentially contains all transformations relative to the camera. This transformation is stored in the `VIEW_MATRIX`. It can also be seen as a transformation that is the inverse of the camera transform, because, relative to the camera, whenever it moves to the right, the object in front of it appears to move to the left, which means that whatever happens to a logical camera can be simulated by performing the inverse operations to each vertex. The `VIEW_MATRIX` is constructed from a position vector and a direction vector through the `LookAt` function, and it encapsulates the translation and rotation (akin to what happens above for instances in `world space`).

After this, geometric clipping is performed for each triangle against each of the camera's 6 planes in `view space` (near, far, left, right, top, and bottom planes).

Now perspective projection is applied to the vertices. At this stage the coordinate system is considered to be in `clip space`.
> Note that the `z` transformation here is non-linear.

Finally, for the 3-dimensional space, perspective division is performed over all vertices by their `w` (which is simply their `z` coordinate prior to the projection).
We are now at what is called `NDC (Normalized Device Coordinates) space`, where, theoretically, every point/vertex that made it to this stage is viewable through the camera and its boundaries are in the ranges [-1, 1] for the x-axis, [-1, 1] for the y-axis, and [0, 1] for the z-axis.

Here, a vertex whose `z` coordinate is the same as the `near` plane value would have a `z` of 0, and if it were to be the same as the `far` plane, it would have a `z` of 1.

We can then start processing the vertices. Here `backface culling` is performed, as well as some [Light](#light) shading (if `Flat` shading it is done entirely here). If `Gouraud` shading, the vertex colors are calculated here for each vertex (they get interpolated at the pixel level later). If `Phong` shading, the vertex normals are calculated for each vertex (these also get interpolated later).

The last coordinate system is `screen space` or `framebuffer space`. Here the vertices' `x` and `y` coordinates are flipped and scaled by the camera's `SCALE_MATRIX`. The `y` axis is flipped because the `y` coordinate increases as you go down when it comes to pixels, but in 3D space it increased as it went up. As for the `x` axis, since I flipped the `z` coordinates when loading the meshes, the resulting logically and semantically correct right-handed coordinate system with this configuration has the positive `x` axis to the left (not to the right), which means that going to the right would instead decrease the `x` coordinate for the pixel. Now we are finally in the 2-dimensional space which represents the window coordinates.

The drawing order is counter-clockwise.

Near plane defaults to `0.01` but can be any value.
Far plane defaults to `1000` but can be any value.

If drawing the lines between vertices, which can be toggled in the menu `Wireframe` checkbox, or programmatically through the `draw_outline` function parameter, the relevant lines are drawn, with the given `Line color` in the [Scene tab](#scene-tab) or programmatically through the `outline_color` parameter.

If filling/rasterizing triangles, the relevant triangles it is done with the `Fill color` in the [Scene tab](#scene-tab) or programmatically through the `fill_color` parameter. This is where the vertices and triangle coordinates are interpolated, and where the fragment shader/texturing/coloring should be implemented.

# Testing
Testing is implemented with `Catch2`.

I implemented some testing in the beginning, but I forgot about it after a while and implementing the main functionality was too time consuming on itself, and I changed everything multiple times, so I eventually gave up on it.

Currently the test covers only most of the `Mat` matrix class and `Util` utility functions.

`TestMain.cpp` sets up and runs the test, it contains the `main` function.
`TestMatrix.cpp` contains matrix related tests.
`TestUtils.cpp` contains utility related tests.

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

- Have a configuration file generated/updated whenever the user closes the program so that everything doesn't need to be redone on the next run. Also include a "Reset" button on the menu in order to reset the settings to the default settings.