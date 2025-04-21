import os
import shutil
import re

def is_valid_game_name(name):
    return bool(re.fullmatch(r"[A-Za-z0-9_]+", name))

def GenerateProject(game_name):
    base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    game_dir = os.path.join(base_dir, "Games", game_name)

    source_dir = os.path.join(game_dir, "source")
    xml_dir = os.path.join(game_dir, "xml")
    results_dir = os.path.join(game_dir, "results")

    for folder in [source_dir, xml_dir, results_dir]:
        os.makedirs(folder, exist_ok=True)

    tools_dir = os.path.join(base_dir, "Tools")
    for filename in ["SimAnalyzer.py", "HistAnalyzer.py"]:
        src = os.path.join(tools_dir, filename)
        dst = os.path.join(results_dir, filename)
        if os.path.exists(src):
            shutil.copyfile(src, dst)
        else:
            print(f"⚠️ Warning: {filename} not found in Tools folder.")

    # Copy C++ source template files and rename GameTemplate → GameName
    template_dir = os.path.join(tools_dir, "GameTemplate")
    for ext in [".cpp", ".h", ".vcxproj", ".vcxproj.filters"]:
        src = os.path.join(template_dir, f"GameTemplate{ext}")
        dst = os.path.join(source_dir, f"{game_name}{ext}")

        if os.path.exists(src):
            with open(src, "r", encoding="utf-8") as f_in:
                content = f_in.read()

            content = content.replace("GameTemplate", game_name)

            with open(dst, "w", encoding="utf-8") as f_out:
                f_out.write(content)
        else:
            print(f"⚠️ Warning: GameTemplate{ext} not found in GameTemplate folder.")


    print(f"✅ Game '{game_name}' structure created successfully.")


def main():
    game_name = input("Enter the new project name: ").strip()

    if not game_name:
        print("❌ No game name entered. Exiting.")
        return

    if not is_valid_game_name(game_name):
        print("❌ Invalid game name. Use only letters, numbers, and underscores.")
        return
    
    GenerateProject(game_name)


if __name__ == "__main__":
    main()
