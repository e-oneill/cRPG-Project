# RPGForGitHub
A Turn Based RPG for Unreal Engine 5.0. My goal was to develop a simple barebones of a CRPG, and implement the 'Delian Tomb' as a level for proof of concept. The gameplay features all work in multiplayer, but I never got around to fixing the UI rendering so it worked in multiplayer, so opening your inventory while in a multiplayer game causes a crash.

# Features:
* Gameplay Action System - A modular system, inspired by the Gameplay Ability System, that allow the creation of gameplay actions in data. This module allows the characters in the game to take actions, handles the display of visual effects like animation, sounds and particle effects. This is fully networked for a multiplayer game.
* Encounter Manager System - Handles the creation of turn-based encounters when hostile creatures spot player controlled characters. Fully networked and working in multiplayer.
* Goal Oriented Action Planning - A custom built AI system that uses a Depth-First-Search for AI Characters to build world models and identify the optimal sequence of 4 actions that will result in a world status desirable to the AI. Heavily depending on the abstractions of the Gameplay Action System to build world models.
