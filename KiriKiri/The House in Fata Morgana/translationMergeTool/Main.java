import java.io.*;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

public class Main {
    public static void main(String[] args) throws IOException {
        // args: executable, f_jpn1, f_chs1, f_jpn2, f_chs2...
        for (String a : args) {
            System.out.println(a);
        }
        int fileCount = (args.length) / 2; // 1 - 0; 2 - 0; 3 - 1;
        for (int idx = 0; idx < fileCount; idx++) {
            String fileNameJpn = args[idx * 2], fileNameChs = args[idx * 2 + 1];
            System.out.println("Now proceeding " + fileNameJpn);

            List<String> fileContentJpn = readWholeFile(fileNameJpn), fileContentChs = readWholeFile(fileNameChs);
            if (fileContentJpn.size() != fileContentChs.size() || fileContentJpn.size() % 2 == 1) {
                System.out.println("File line count illegal: " + fileContentJpn.size() + " vs " + fileContentChs.size());
                System.out.println("Skipped!");
                continue;
            }

            // do the operations
            for (int i = 0; i < fileContentJpn.size(); i += 2) {
                String translatedLine = fileContentJpn.get(i + 1).substring(0, fileContentJpn.get(i + 1).lastIndexOf("●"))
                        + fileContentChs.get(i).substring(fileContentChs.get(i).lastIndexOf("○"));
                fileContentJpn.set(i + 1, translatedLine);
            }

            // save file
            String fileNameSave = fileNameJpn.substring(0, fileNameJpn.indexOf('.')) + "_converted.txt";
            System.out.println("Saving to " + fileNameSave);
            saveWholeFile(fileNameSave, fileContentJpn);
        }

    }

    private static List<String> readWholeFile(String fileName) throws IOException {
        List<String> result = new ArrayList<>();
        File file = new File(fileName);
        FileInputStream is = new FileInputStream(file);
        BufferedReader reader = new BufferedReader(new InputStreamReader(is, Charset.forName("UTF-16")));

        while (true) {
            String temp = reader.readLine();
            if(temp == null) break;
            if(temp.length() < 5) continue;
            result.add(temp);
        }
        return result;
    }

    private static void saveWholeFile(String fileName, List<String> fileContent) throws IOException {
        FileOutputStream outStream = new FileOutputStream(fileName);
        StringBuilder stringBuilder = new StringBuilder();
        for(int i = 0; i < fileContent.size(); i += 2) {
            stringBuilder.append(fileContent.get(i) + "\r\n" + fileContent.get(i + 1) + "\r\n\r\n");
        }
        outStream.write(stringBuilder.toString().getBytes("UTF-16"));
        outStream.close();
    }
}
