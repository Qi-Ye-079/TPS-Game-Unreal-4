# Third Person Shooter Project - game name undetermined yet

This is my personal project - a TPS being game made by Unreal Engine 4 - used for programming practice and personal interest. The current version of Unreal is 4.19.2 with Visual Studio 2017 v15.6.7.

All the art assets and generated cache folders are not being tracked due to large file size and privacy issues. Refer to .gitignore file for details.

Because some content assets are ignored, every time this repo is cloned, besides rebuilding the project and source codes, the following assets are needed to put into the Content folder: 

1.	[WeaponMeshes.zip](https://udemy-assets-on-demand.udemy.com/2017-12-10_02-32-19-c0a08a60e0db190722a4c55c42b3daf7/original.zip?nva=20180525083041&filename=WeaponMeshes.zip&download=True&token=0abd137ed2698519ca80f) - for weapon's skeletal mesh (from Udemy)
2.	[WeaponEffectsPack.zip](https://udemy-assets-on-demand.udemy.com/2017-12-10_02-34-09-105febccfd62b4bc01fb8cc081c167e3/original.zip?nva=20180525083240&filename=WeaponEffectsPack.zip&download=True&token=0b5ebcf11da96099e683a) - for weapon's muzzle, blood and impact effects  (from Udemy)
3.	The [Animation Starter Pack](https://www.unrealengine.com/marketplace/animation-starter-pack) is also needed for testing character's animation. It's free!

**NOTES:** 

1.	don't forget to add a **WeaponSocket** on the **hand_r** socket on the Skeleton of the SK_Mannequin_Skeleton and carefully adjust its transformation, as the correct spawing of the weapon needs it.
2.	Duplicate **UE4ASP_HeroTPP_AnimBlueprint** and name it **Custom_Anim_BP**, which will be used for player character's animation blueprint.

**Documentations that may be helpful:**

1.	[Skeletal Mesh Animation System](https://docs.unrealengine.com/en-us/Engine/Animation)
2.	[C++ FPS tutorial](http://api.unrealengine.com/latest/INT/Programming/Tutorials/FirstPersonShooter/index.html) - include projectile implementation.
3.	[Components and Collision](http://api.unrealengine.com/latest/INT/Programming/Tutorials/Components/index.html)
4.	[User Interface With UMG](http://api.unrealengine.com/latest/INT/Programming/Tutorials/UMG/index.html)
5.	[Variables, Timers, and Events](http://api.unrealengine.com/latest/INT/Programming/Tutorials/VariablesTimersEvents/index.html) - includes using Timer for automatic weapon
6.	[Add a Surface Type](https://docs.unrealengine.com/en-us/Engine/Physics/PhysicalMaterials/HowTo/AddSurfaceType)
7.	[Decals](https://docs.unrealengine.com/en-us/Resources/ContentExamples/Decals) - Information about decal materials

