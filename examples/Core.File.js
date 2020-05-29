const path = "/home/anon/myfile.txt";
const file = new Core.File(path);
if (!file.open(Core.File.OpenMode.READ_ONLY)) {
    console.log("Failed to open:", path, file.errorString);
    exit(1);
}
const text = file.readAll();
console.log(text);
