<div align="center">

# RNA-PID — Neural Network & Process Control Study

**An experimental study combining neural-network modeling, simulation, and PID control.**

![Domain](https://img.shields.io/badge/domain-control%20engineering-0A7F8C)
![Methods](https://img.shields.io/badge/methods-neural%20networks%20%7C%20PID-6f42c1)
![Status](https://img.shields.io/badge/status-research%20project-orange)

</div>

## Overview

RNA-PID explores a control-engineering workflow that brings together process modeling, simulation, neural-network configuration, and controller tuning.

The repository includes project notes and a folder dedicated to a magnetic-levitation bench. Its documentation describes the variables and experiment parameters considered during the study.

## Study workflow

The documented workflow is organized around four areas:

1. **Process definition** — define process variables and dispersion-related parameters.
2. **Simulation** — configure input types, input parameters, combinations, and simulation time.
3. **Neural network** — define input/output dimensions, hidden-layer topology, activation functions, training parameters, metrics, and normalization.
4. **Controller** — select a transfer-function form, observed variables, and a tuning method.

## Repository contents

```text
.
├── bancada_maglev_h/          # Magnetic-levitation bench materials
├── matTrab/                   # Supporting study materials
├── variaveis_do_processo.txt  # Process, model, and controller notes
└── README.md
```

## Tools and concepts

The notes refer to MATLAB scripting and Simulink simulation, neural-network modeling, and classical controller design. Specific software versions and installation procedures are not currently documented.

## Getting started

Start by reviewing `variaveis_do_processo.txt` to understand the process variables and configuration choices. Then inspect the contents of `bancada_maglev_h/` and `matTrab/` to identify the relevant experiment files.

The repository does not currently provide a verified, one-command setup procedure.

## Scope and limitations

This README describes the documented study workflow. It does not claim that a trained network, tuned controller, or complete reproducible experiment is packaged and validated.

## Project status

Research / experimental project.

## License

No license file was identified in the repository. Contact the author before redistributing or reusing these materials.
