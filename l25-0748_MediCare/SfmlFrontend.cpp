#include "SfmlFrontend.h"

#if defined(MEDICORE_ENABLE_SFML)

#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "CoreStrings.h"
#include "Doctor.h"
#include "FileHandler.h"
#include "FileNotFoundException.h"
#include "Patient.h"
#include "Prescription.h"
#include "SfmlAdminMenu.h"
#include "SfmlDoctorMenu.h"
#include "SfmlLoginMenu.h"
#include "SfmlPatientMenu.h"
#include "Storage.h"
#include "Validator.h"

#include <SFML/Graphics.hpp>
#include <sstream>
#include <string>
#include <vector>

enum SfmlScreen
{
    SFML_ROLE_SCREEN = 0,
    SFML_LOGIN_ID_SCREEN = 1,
    SFML_LOGIN_PASSWORD_SCREEN = 2,
    SFML_PATIENT_MENU_SCREEN = 3,
    SFML_DOCTOR_MENU_SCREEN = 4,
    SFML_ADMIN_MENU_SCREEN = 5,
    SFML_ACTION_SCREEN = 6
};

struct SfmlButton
{
    sf::RectangleShape box;
    sf::Text text;
};

struct SfmlInput
{
    sf::RectangleShape box;
    sf::Text text;
    sf::String value;
    int maxLen;
};

struct SfmlAction
{
    int option;
    int role;
    int promptIndex;
    std::vector<std::string> prompts;
    std::vector<std::string> answers;
};

static const char* const SFML_FIXED_SLOTS[8] = { "09:00", "10:00", "11:00", "12:00",
                                                "13:00", "14:00", "15:00", "16:00" };

static void sfmlButtonSetup(SfmlButton* b, const sf::Font& font, const char* caption, float x, float y, float w,
    float h)
{
    b->box.setPosition(x, y);
    b->box.setSize(sf::Vector2f(w, h));
    b->box.setFillColor(sf::Color(49, 106, 166));
    b->box.setOutlineColor(sf::Color(210, 225, 245));
    b->box.setOutlineThickness(1.0f);

    b->text.setFont(font);
    b->text.setString(caption);
    b->text.setCharacterSize(18);
    b->text.setFillColor(sf::Color::White);
    sf::FloatRect r = b->text.getLocalBounds();
    b->text.setPosition(x + (w - r.width) * 0.5f - r.left, y + (h - r.height) * 0.5f - r.top);
}

static void sfmlButtonSetCaption(SfmlButton* b, const char* caption)
{
    b->text.setString(caption);
    sf::Vector2f p = b->box.getPosition();
    sf::Vector2f s = b->box.getSize();
    sf::FloatRect r = b->text.getLocalBounds();
    b->text.setPosition(p.x + (s.x - r.width) * 0.5f - r.left, p.y + (s.y - r.height) * 0.5f - r.top);
}

static bool sfmlButtonHit(const SfmlButton* b, float x, float y)
{
    return b->box.getGlobalBounds().contains(x, y);
}

static void sfmlButtonDraw(sf::RenderWindow* w, const SfmlButton* b)
{
    w->draw(b->box);
    w->draw(b->text);
}

static void sfmlInputSetup(SfmlInput* in, const sf::Font& font, float x, float y, float w, float h, int maxLen)
{
    in->box.setPosition(x, y);
    in->box.setSize(sf::Vector2f(w, h));
    in->box.setFillColor(sf::Color(14, 19, 27));
    in->box.setOutlineColor(sf::Color(210, 225, 245));
    in->box.setOutlineThickness(1.0f);
    in->text.setFont(font);
    in->text.setCharacterSize(19);
    in->text.setFillColor(sf::Color::White);
    in->text.setPosition(x + 10.0f, y + 8.0f);
    in->value.clear();
    in->maxLen = maxLen;
}

static void sfmlInputClear(SfmlInput* in)
{
    in->value.clear();
    in->text.setString("");
}

static std::string sfmlInputValue(const SfmlInput* in)
{
    return in->value.toAnsiString();
}

static void sfmlInputHandleText(SfmlInput* in, const sf::Event* event)
{
    if (event->type != sf::Event::TextEntered)
    {
        return;
    }

    unsigned int code = event->text.unicode;
    if (code == 8)
    {
        if (!in->value.isEmpty())
        {
            in->value.erase(in->value.getSize() - 1, 1);
        }
    }
    else if (code >= 32 && code <= 126)
    {
        if ((int)in->value.getSize() < in->maxLen)
        {
            in->value += (char)code;
        }
    }
    in->text.setString(in->value);
}

static void sfmlCopyToBuffer(const std::string& s, char* out, int cap)
{
    if (cap <= 0)
    {
        return;
    }
    int i = 0;
    while (i < cap - 1 && i < (int)s.size())
    {
        out[i] = s[(std::size_t)i];
        i++;
    }
    out[i] = '\0';
}

static void sfmlLoadAll(Storage<Patient>* patients, Storage<Doctor>* doctors, Admin* admin,
    Storage<Appointment>* appointments, Storage<Bill>* bills,
    Storage<Prescription>* prescriptions)
{
    FileHandler::loadPatients(patients);
    FileHandler::loadDoctors(doctors);
    FileHandler::loadAdmin(admin);
    FileHandler::loadAppointments(appointments);
    FileHandler::loadBills(bills);
    FileHandler::loadPrescriptions(prescriptions);
}

static void sfmlFreeAll(Storage<Patient>* patients, Storage<Doctor>* doctors, Admin* admin,
    Storage<Appointment>* appointments, Storage<Bill>* bills,
    Storage<Prescription>* prescriptions)
{
    delete patients;
    delete doctors;
    delete admin;
    delete appointments;
    delete bills;
    delete prescriptions;
}

static void sfmlAppendWrapped(std::vector<std::string>* lines, const std::string& line)
{
    const int width = 100;
    if ((int)line.size() <= width)
    {
        lines->push_back(line);
        return;
    }

    int start = 0;
    while (start < (int)line.size())
    {
        int count = width;
        if (start + count > (int)line.size())
        {
            count = (int)line.size() - start;
        }
        lines->push_back(line.substr((std::size_t)start, (std::size_t)count));
        start += count;
    }
}

static void sfmlAppendText(std::vector<std::string>* lines, const std::string& text)
{
    std::stringstream ss(text);
    std::string line;
    while (std::getline(ss, line))
    {
        sfmlAppendWrapped(lines, line);
    }
}

static std::string sfmlVisibleTranscript(const std::vector<std::string>& lines)
{
    int start = 0;
    const int maxLines = 24;
    if ((int)lines.size() > maxLines)
    {
        start = (int)lines.size() - maxLines;
    }

    std::string out;
    int i = start;
    while (i < (int)lines.size())
    {
        out += lines[(std::size_t)i];
        if (i + 1 < (int)lines.size())
        {
            out += "\n";
        }
        i++;
    }
    return out;
}

static void sfmlShowRoleMenu(std::vector<std::string>* lines)
{
    lines->clear();
    sfmlAppendText(lines, "Welcome to MediCore Hospital Management System");
    sfmlAppendText(lines, "===============================================");
    sfmlAppendText(lines, "Login as:");
    sfmlAppendText(lines, "1. Patient");
    sfmlAppendText(lines, "2. Doctor");
    sfmlAppendText(lines, "3. Admin");
    sfmlAppendText(lines, "4. Exit");
}

static void sfmlShowPatientMenu(std::vector<std::string>* lines, Patient* patient)
{
    sfmlAppendText(lines, "");
    sfmlAppendText(lines, std::string("Welcome, ") + patient->getNameConst());
    std::ostringstream bal;
    bal << "Balance: PKR " << patient->getBalance();
    sfmlAppendText(lines, bal.str());
    sfmlAppendText(lines, "========================");
    sfmlAppendText(lines, "1. Book Appointment");
    sfmlAppendText(lines, "2. Cancel Appointment");
    sfmlAppendText(lines, "3. View My Appointments");
    sfmlAppendText(lines, "4. View My Medical Records");
    sfmlAppendText(lines, "5. View My Bills");
    sfmlAppendText(lines, "6. Pay Bill");
    sfmlAppendText(lines, "7. Top Up Balance");
    sfmlAppendText(lines, "8. Logout");
}

static void sfmlShowDoctorMenu(std::vector<std::string>* lines, Doctor* doctor)
{
    sfmlAppendText(lines, "");
    sfmlAppendText(lines, std::string("Welcome, Dr. ") + doctor->getNameConst());
    sfmlAppendText(lines, std::string("Specialization: ") + doctor->getSpecializationConst());
    sfmlAppendText(lines, "===============================================");
    sfmlAppendText(lines, "1. View Today's Appointments");
    sfmlAppendText(lines, "2. Mark Appointment Complete");
    sfmlAppendText(lines, "3. Mark Appointment No-Show");
    sfmlAppendText(lines, "4. Write Prescription");
    sfmlAppendText(lines, "5. View Patient Medical History");
    sfmlAppendText(lines, "6. Logout");
}

static void sfmlShowAdminMenu(std::vector<std::string>* lines)
{
    sfmlAppendText(lines, "");
    sfmlAppendText(lines, "Admin Panel - MediCore");
    sfmlAppendText(lines, "======================");
    sfmlAppendText(lines, "1. Add Doctor");
    sfmlAppendText(lines, "2. Remove Doctor");
    sfmlAppendText(lines, "3. View All Patients");
    sfmlAppendText(lines, "4. View All Doctors");
    sfmlAppendText(lines, "5. View All Appointments");
    sfmlAppendText(lines, "6. View Unpaid Bills");
    sfmlAppendText(lines, "7. Discharge Patient");
    sfmlAppendText(lines, "8. View Security Log");
    sfmlAppendText(lines, "9. Generate Daily Report");
    sfmlAppendText(lines, "10. Logout");
}

static bool sfmlSlotConflicts(int doctorId, const char* dateStr, const char* slotStr,
    Storage<Appointment>* appointments)
{
    int n = appointments->size();
    Appointment** arr = appointments->getAll();
    int i = 0;
    while (i < n)
    {
        Appointment* a = arr[i];
        if (a != nullptr && a->getDoctorID() == doctorId && core_str_cmp(a->getDateConst(), dateStr) == 0 &&
            core_str_cmp(a->getTimeSlotConst(), slotStr) == 0 && core_str_cmp(a->getStatusConst(), "cancelled") != 0)
        {
            return true;
        }
        i++;
    }
    return false;
}

static void sfmlPreviewPatientStep(SfmlAction* action, std::vector<std::string>* lines, Storage<Doctor>* doctors,
    Storage<Appointment>* appointments, Storage<Bill>* bills, Patient* patient)
{
    if (action->role != 1)
    {
        return;
    }

    if (action->option == 1 && action->promptIndex == 1)
    {
        const std::string spec = action->answers[0];
        sfmlAppendText(lines, "Matching doctors:");
        int n = doctors->size();
        Doctor** arr = doctors->getAll();
        bool any = false;
        int i = 0;
        while (i < n)
        {
            Doctor* d = arr[i];
            if (d != nullptr && Validator::caseInsensitiveCompare(d->getSpecialization(), spec.c_str()))
            {
                any = true;
                std::ostringstream row;
                row << "ID " << d->getId() << " | " << d->getNameConst() << " | Fee PKR " << d->getFee();
                sfmlAppendText(lines, row.str());
            }
            i++;
        }
        if (!any)
        {
            sfmlAppendText(lines, "No doctors available for that specialization.");
        }
    }
    else if (action->option == 1 && action->promptIndex == 3)
    {
        int doctorId = core_parse_int(action->answers[1].c_str());
        const char* dateStr = action->answers[2].c_str();
        sfmlAppendText(lines, "Available slots:");
        int i = 0;
        while (i < 8)
        {
            if (!sfmlSlotConflicts(doctorId, dateStr, SFML_FIXED_SLOTS[i], appointments))
            {
                sfmlAppendText(lines, SFML_FIXED_SLOTS[i]);
            }
            i++;
        }
    }
    else if (action->option == 2 && action->promptIndex == 0)
    {
        sfmlAppendText(lines, "Pending appointments:");
        int n = appointments->size();
        Appointment** arr = appointments->getAll();
        bool any = false;
        int i = 0;
        while (i < n)
        {
            Appointment* a = arr[i];
            if (a != nullptr && a->getPatientID() == patient->getId() &&
                core_str_cmp(a->getStatusConst(), "pending") == 0)
            {
                any = true;
                Doctor* d = doctors->findByID(a->getDoctorID());
                const char* docName = (d != nullptr) ? d->getNameConst() : "Unknown";
                std::ostringstream row;
                row << "Appointment ID " << a->getAppointmentID() << " | Doctor " << docName << " | Date "
                    << a->getDateConst() << " | Slot " << a->getTimeSlotConst();
                sfmlAppendText(lines, row.str());
            }
            i++;
        }
        if (!any)
        {
            sfmlAppendText(lines, "You have no pending appointments.");
        }
    }
    else if (action->option == 6 && action->promptIndex == 0)
    {
        sfmlAppendText(lines, "Unpaid bills:");
        int n = bills->size();
        Bill** arr = bills->getAll();
        bool any = false;
        int i = 0;
        while (i < n)
        {
            Bill* b = arr[i];
            if (b != nullptr && b->getPatientID() == patient->getId() &&
                core_str_cmp(b->getStatusConst(), "unpaid") == 0)
            {
                any = true;
                std::ostringstream row;
                row << "Bill ID " << b->getBillID() << " | Appt " << b->getAppointmentID() << " | Amount PKR "
                    << b->getAmount();
                sfmlAppendText(lines, row.str());
            }
            i++;
        }
        if (!any)
        {
            sfmlAppendText(lines, "No unpaid bills.");
        }
    }
}

static void sfmlPreviewDoctorStep(SfmlAction* action, std::vector<std::string>* lines,
    Storage<Appointment>* appointments, Doctor* doctor)
{
    if (action->role != 2 || (action->option != 2 && action->option != 3) || action->promptIndex != 0)
    {
        return;
    }

    char todayBuf[16];
    core_get_today_dd_mm_yyyy(todayBuf);
    sfmlAppendText(lines, "Today's pending appointments:");
    int n = appointments->size();
    Appointment** arr = appointments->getAll();
    bool any = false;
    int i = 0;
    while (i < n)
    {
        Appointment* a = arr[i];
        if (a != nullptr && a->getDoctorID() == doctor->getId() &&
            core_str_cmp(a->getDateConst(), todayBuf) == 0 && core_str_cmp(a->getStatusConst(), "pending") == 0)
        {
            any = true;
            std::ostringstream row;
            row << "ID " << a->getAppointmentID() << " | Slot " << a->getTimeSlotConst();
            sfmlAppendText(lines, row.str());
        }
        i++;
    }
    if (!any)
    {
        sfmlAppendText(lines, "No pending appointments for today.");
    }
}

static void sfmlPreviewAdminStep(SfmlAction* action, std::vector<std::string>* lines, Storage<Doctor>* doctors)
{
    if (action->role != 3 || action->option != 2 || action->promptIndex != 0)
    {
        return;
    }

    sfmlAppendText(lines, "Doctors list:");
    int n = doctors->size();
    Doctor** arr = doctors->getAll();
    int i = 0;
    while (i < n)
    {
        Doctor* d = arr[i];
        if (d != nullptr)
        {
            std::ostringstream row;
            row << d->getId() << " | " << d->getNameConst() << " | " << d->getSpecializationConst()
                << " | Fee PKR " << d->getFee();
            sfmlAppendText(lines, row.str());
        }
        i++;
    }
}

static std::vector<std::string> sfmlPromptsForAction(int role, int option)
{
    std::vector<std::string> prompts;
    if (role == 1)
    {
        if (option == 1)
        {
            prompts.push_back("Enter specialization to search (e.g. Cardiology):");
            prompts.push_back("Enter Doctor ID:");
            prompts.push_back("Enter date (DD-MM-YYYY):");
            prompts.push_back("Enter time slot (e.g. 09:00):");
        }
        else if (option == 2)
        {
            prompts.push_back("Enter Appointment ID to cancel:");
        }
        else if (option == 6)
        {
            prompts.push_back("Enter Bill ID to pay:");
        }
        else if (option == 7)
        {
            prompts.push_back("Enter amount to add (PKR):");
        }
    }
    else if (role == 2)
    {
        if (option == 2 || option == 3 || option == 4)
        {
            prompts.push_back("Enter Appointment ID:");
        }
        if (option == 4)
        {
            prompts.push_back("Enter medicines (Medicine Dosage;Medicine2 Dosage):");
            prompts.push_back("Enter notes (max 300 chars):");
        }
        else if (option == 5)
        {
            prompts.push_back("Enter Patient ID:");
        }
    }
    else if (role == 3)
    {
        if (option == 1)
        {
            prompts.push_back("Doctor full name (max 50 chars):");
            prompts.push_back("Specialization (max 50 chars):");
            prompts.push_back("Contact (11 digits):");
            prompts.push_back("Password (min 6 chars):");
            prompts.push_back("Consultation fee (PKR):");
        }
        else if (option == 2)
        {
            prompts.push_back("Enter Doctor ID to remove:");
        }
        else if (option == 7)
        {
            prompts.push_back("Enter Patient ID:");
        }
    }
    return prompts;
}

static std::string sfmlBuildScript(const std::vector<std::string>& answers)
{
    std::string script;
    int i = 0;
    while (i < (int)answers.size())
    {
        if (i > 0)
        {
            script += "|";
        }
        script += answers[(std::size_t)i];
        i++;
    }
    return script;
}

static void sfmlRunAction(SfmlAction* action, Patient* loggedPatient, Doctor* loggedDoctor, Admin* loggedAdmin,
    Storage<Patient>* patients, Storage<Doctor>* doctors, Storage<Appointment>* appointments,
    Storage<Bill>* bills, Storage<Prescription>* prescriptions, std::vector<std::string>* lines)
{
    char outputBuf[8000];
    outputBuf[0] = '\0';
    std::string script = sfmlBuildScript(action->answers);

    if (action->role == 1)
    {
        sfmlPatientRunOption(action->option, script.c_str(), loggedPatient, doctors, appointments, bills,
            prescriptions, outputBuf, 8000);
    }
    else if (action->role == 2)
    {
        sfmlDoctorRunOption(action->option, script.c_str(), loggedDoctor, appointments, patients, prescriptions,
            bills, outputBuf, 8000);
    }
    else if (action->role == 3)
    {
        sfmlAdminRunOption(action->option, script.c_str(), loggedAdmin, patients, doctors, appointments, bills,
            prescriptions, outputBuf, 8000);
    }

    sfmlAppendText(lines, outputBuf);
}

static void sfmlDraw(sf::RenderWindow* window, sf::Text* title, sf::Text* transcriptText, sf::Text* promptText,
    SfmlInput* input, SfmlButton* submit, SfmlButton* clear, SfmlButton* back)
{
    window->clear(sf::Color(22, 28, 38));
    window->draw(*title);
    window->draw(*transcriptText);
    window->draw(*promptText);
    window->draw(input->box);
    window->draw(input->text);
    sfmlButtonDraw(window, submit);
    sfmlButtonDraw(window, clear);
    sfmlButtonDraw(window, back);
    window->display();
}

void runMediCoreSfmlFrontend()
{
    Storage<Patient>* patients = new Storage<Patient>();
    Storage<Doctor>* doctors = new Storage<Doctor>();
    Admin* admin = new Admin();
    Storage<Appointment>* appointments = new Storage<Appointment>();
    Storage<Bill>* bills = new Storage<Bill>();
    Storage<Prescription>* prescriptions = new Storage<Prescription>();

    try
    {
        sfmlLoadAll(patients, doctors, admin, appointments, bills, prescriptions);
    }
    catch (FileNotFoundException&)
    {
        sfmlFreeAll(patients, doctors, admin, appointments, bills, prescriptions);
        return;
    }

    sf::RenderWindow window(sf::VideoMode(1040, 760), "MediCore - SFML Mode");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        sfmlFreeAll(patients, doctors, admin, appointments, bills, prescriptions);
        return;
    }

    sf::Text title("MediCore Hospital Management System", font, 28);
    title.setPosition(24.0f, 18.0f);
    title.setFillColor(sf::Color::White);

    sf::Text transcriptText("", font, 17);
    transcriptText.setPosition(24.0f, 66.0f);
    transcriptText.setFillColor(sf::Color(230, 236, 248));

    sf::Text promptText("Enter choice:", font, 20);
    promptText.setPosition(24.0f, 600.0f);
    promptText.setFillColor(sf::Color(245, 220, 130));

    SfmlInput input;
    sfmlInputSetup(&input, font, 24.0f, 632.0f, 676.0f, 42.0f, 220);

    SfmlButton submit;
    SfmlButton clear;
    SfmlButton back;
    sfmlButtonSetup(&submit, font, "Submit", 720.0f, 632.0f, 130.0f, 42.0f);
    sfmlButtonSetup(&clear, font, "Clear", 866.0f, 632.0f, 130.0f, 42.0f);
    sfmlButtonSetup(&back, font, "Exit", 866.0f, 690.0f, 130.0f, 42.0f);

    SfmlScreen screen = SFML_ROLE_SCREEN;
    SfmlAction action;
    int selectedRole = 0;
    int loginFailures = 0;
    std::string loginId;
    Patient* loggedPatient = nullptr;
    Doctor* loggedDoctor = nullptr;
    Admin* loggedAdmin = nullptr;
    std::vector<std::string> lines;
    sfmlShowRoleMenu(&lines);

    bool requestSubmit = false;

    while (window.isOpen())
    {
        if (screen == SFML_ROLE_SCREEN)
        {
            promptText.setString("Enter choice:");
            sfmlButtonSetCaption(&back, "Exit");
        }
        else if (screen == SFML_LOGIN_ID_SCREEN)
        {
            promptText.setString("Enter User ID:");
            sfmlButtonSetCaption(&back, "Back");
        }
        else if (screen == SFML_LOGIN_PASSWORD_SCREEN)
        {
            promptText.setString("Enter Password:");
            sfmlButtonSetCaption(&back, "Back");
        }
        else if (screen == SFML_PATIENT_MENU_SCREEN || screen == SFML_DOCTOR_MENU_SCREEN ||
            screen == SFML_ADMIN_MENU_SCREEN)
        {
            promptText.setString("Select option:");
            sfmlButtonSetCaption(&back, "Logout");
        }
        else
        {
            promptText.setString(action.prompts[(std::size_t)action.promptIndex]);
            sfmlButtonSetCaption(&back, "Cancel");
        }

        transcriptText.setString(sfmlVisibleTranscript(lines));

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::TextEntered)
            {
                sfmlInputHandleText(&input, &event);
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return)
            {
                requestSubmit = true;
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                float mx = (float)event.mouseButton.x;
                float my = (float)event.mouseButton.y;
                if (sfmlButtonHit(&submit, mx, my))
                {
                    requestSubmit = true;
                }
                else if (sfmlButtonHit(&clear, mx, my))
                {
                    sfmlInputClear(&input);
                }
                else if (sfmlButtonHit(&back, mx, my))
                {
                    if (screen == SFML_ROLE_SCREEN)
                    {
                        window.close();
                    }
                    else if (screen == SFML_ACTION_SCREEN)
                    {
                        sfmlAppendText(&lines, "Action cancelled.");
                        if (action.role == 1)
                        {
                            screen = SFML_PATIENT_MENU_SCREEN;
                            sfmlShowPatientMenu(&lines, loggedPatient);
                        }
                        else if (action.role == 2)
                        {
                            screen = SFML_DOCTOR_MENU_SCREEN;
                            sfmlShowDoctorMenu(&lines, loggedDoctor);
                        }
                        else
                        {
                            screen = SFML_ADMIN_MENU_SCREEN;
                            sfmlShowAdminMenu(&lines);
                        }
                    }
                    else
                    {
                        loggedPatient = nullptr;
                        loggedDoctor = nullptr;
                        loggedAdmin = nullptr;
                        selectedRole = 0;
                        loginId.clear();
                        screen = SFML_ROLE_SCREEN;
                        sfmlShowRoleMenu(&lines);
                    }
                    sfmlInputClear(&input);
                }
            }
        }

        if (requestSubmit)
        {
            requestSubmit = false;
            std::string value = sfmlInputValue(&input);
            sfmlInputClear(&input);

            if (screen == SFML_ROLE_SCREEN)
            {
                sfmlAppendText(&lines, std::string("Enter choice: ") + value);
                int choice = core_parse_int(value.c_str());
                if (choice >= 1 && choice <= 3)
                {
                    selectedRole = choice;
                    screen = SFML_LOGIN_ID_SCREEN;
                }
                else if (choice == 4)
                {
                    window.close();
                }
                else
                {
                    sfmlAppendText(&lines, "Invalid choice.");
                }
            }
            else if (screen == SFML_LOGIN_ID_SCREEN)
            {
                loginId = value;
                sfmlAppendText(&lines, std::string("Enter User ID: ") + value);
                screen = SFML_LOGIN_PASSWORD_SCREEN;
            }
            else if (screen == SFML_LOGIN_PASSWORD_SCREEN)
            {
                sfmlAppendText(&lines, "Enter Password: ********");
                std::string loginScript = loginId + "|" + value;
                char outputBuf[4000];
                int code = sfmlLoginTry(loginScript.c_str(), selectedRole, patients, doctors, admin, &loggedPatient,
                    &loggedDoctor, &loggedAdmin, &loginFailures, outputBuf, 4000);
                sfmlAppendText(&lines, outputBuf);
                if (code == 1)
                {
                    if (selectedRole == 1)
                    {
                        screen = SFML_PATIENT_MENU_SCREEN;
                        sfmlShowPatientMenu(&lines, loggedPatient);
                    }
                    else if (selectedRole == 2)
                    {
                        screen = SFML_DOCTOR_MENU_SCREEN;
                        sfmlShowDoctorMenu(&lines, loggedDoctor);
                    }
                    else
                    {
                        screen = SFML_ADMIN_MENU_SCREEN;
                        sfmlShowAdminMenu(&lines);
                    }
                }
                else
                {
                    screen = SFML_LOGIN_ID_SCREEN;
                }
            }
            else if (screen == SFML_PATIENT_MENU_SCREEN || screen == SFML_DOCTOR_MENU_SCREEN ||
                screen == SFML_ADMIN_MENU_SCREEN)
            {
                sfmlAppendText(&lines, std::string("Select option: ") + value);
                int option = core_parse_int(value.c_str());
                int role = (screen == SFML_PATIENT_MENU_SCREEN) ? 1 : ((screen == SFML_DOCTOR_MENU_SCREEN) ? 2 : 3);
                int maxOption = (role == 1) ? 8 : ((role == 2) ? 6 : 10);
                if (option < 1 || option > maxOption)
                {
                    sfmlAppendText(&lines, "Invalid menu choice.");
                }
                else if (option == maxOption)
                {
                    loggedPatient = nullptr;
                    loggedDoctor = nullptr;
                    loggedAdmin = nullptr;
                    screen = SFML_ROLE_SCREEN;
                    sfmlShowRoleMenu(&lines);
                }
                else
                {
                    action.option = option;
                    action.role = role;
                    action.promptIndex = 0;
                    action.answers.clear();
                    action.prompts = sfmlPromptsForAction(role, option);
                    if (action.prompts.empty())
                    {
                        sfmlRunAction(&action, loggedPatient, loggedDoctor, loggedAdmin, patients, doctors,
                            appointments, bills, prescriptions, &lines);
                        if (role == 1)
                        {
                            sfmlShowPatientMenu(&lines, loggedPatient);
                        }
                        else if (role == 2)
                        {
                            sfmlShowDoctorMenu(&lines, loggedDoctor);
                        }
                        else
                        {
                            sfmlShowAdminMenu(&lines);
                        }
                    }
                    else
                    {
                        screen = SFML_ACTION_SCREEN;
                        sfmlPreviewPatientStep(&action, &lines, doctors, appointments, bills, loggedPatient);
                        sfmlPreviewDoctorStep(&action, &lines, appointments, loggedDoctor);
                        sfmlPreviewAdminStep(&action, &lines, doctors);
                    }
                }
            }
            else if (screen == SFML_ACTION_SCREEN)
            {
                sfmlAppendText(&lines, action.prompts[(std::size_t)action.promptIndex] + " " + value);
                action.answers.push_back(value);
                action.promptIndex++;
                if (action.promptIndex >= (int)action.prompts.size())
                {
                    sfmlRunAction(&action, loggedPatient, loggedDoctor, loggedAdmin, patients, doctors, appointments,
                        bills, prescriptions, &lines);
                    if (action.role == 1)
                    {
                        screen = SFML_PATIENT_MENU_SCREEN;
                        sfmlShowPatientMenu(&lines, loggedPatient);
                    }
                    else if (action.role == 2)
                    {
                        screen = SFML_DOCTOR_MENU_SCREEN;
                        sfmlShowDoctorMenu(&lines, loggedDoctor);
                    }
                    else
                    {
                        screen = SFML_ADMIN_MENU_SCREEN;
                        sfmlShowAdminMenu(&lines);
                    }
                }
                else
                {
                    sfmlPreviewPatientStep(&action, &lines, doctors, appointments, bills, loggedPatient);
                    sfmlPreviewDoctorStep(&action, &lines, appointments, loggedDoctor);
                    sfmlPreviewAdminStep(&action, &lines, doctors);
                }
            }
        }

        sfmlDraw(&window, &title, &transcriptText, &promptText, &input, &submit, &clear, &back);
    }

    sfmlFreeAll(patients, doctors, admin, appointments, bills, prescriptions);
}

#else

void runMediCoreSfmlFrontend()
{
}

#endif
