workspace "AxiomEngine"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Theorem"

    OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

    group "Axiom"
        include "Axiom/Build-Axiom.lua"
    group ""

    include "Theorem/Build-Theorem.lua"
