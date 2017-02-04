// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

// Integrate Paper2D actions associated with existing engine types (e.g., Texture2D) into the content browser
class FJCContentBrowserExtensions
{
public:
    static void InstallHooks();
    static void RemoveHooks();
};