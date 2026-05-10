# MediCore Hospital Management System

MediCore is a C++ hospital management system with two startup modes:

1. Console Mode
2. SFML GUI Mode

The system supports login for Patients, Doctors, and Admin. It manages appointments, bills, prescriptions, doctors, patients, and daily reports using local `.txt` files.

## Requirements

- Visual Studio with C++ support
- SFML 2.6 configured in the project
- All project `.cpp` and `.h` files included
- The data files must stay in the same folder as the executable/project working directory:
  - `patients.txt`
  - `doctors.txt`
  - `admin.txt`
  - `appointments.txt`
  - `bills.txt`
  - `prescriptions.txt`
  - `security_log.txt`

## Running the Program

Build and run the project from Visual Studio in RELEASE MODE

When the program starts, it asks:

```text
Select mode:
1. Console
2. SFML
Enter choice:

## Demo credentials

| Role   | ID | Password  |
|--------|----|-----------|
| Patient | 1 | pass123 |
| Patient | 2 | pass456 |
| Doctor  | 1 | doc456 |
| Doctor  | 2 | doc123 |
| Doctor  | 3 | doc789 |
| Admin   | 1 | admin123 |

## GitHub repository

https://github.com/SHADOWWW20/l25-0748_MediCare_Proj

## Notes

- Booking dates must be **DD-MM-YYYY** with year **greater than or equal to the current calendar year** (validated via `Validator`).
- Sample seed data uses **2026** dates so exercises align with current-year validation during grading.
- `PatientMenu::bookAppointment` / `cancelAppointment` take a `Storage<Bill>*` argument so bills stay synchronized with disk operations while appointments change.
