import QtQuick 2.0

ComboBox {
    function nameWeight(str) {
        //Returns weight for given str, used in sorting
        switch(str) {

        //Source always on top
        case "source": return 999999;

        //Rest in order
        case "audio_only": return -1;
        case "mobile": return 0;
        case "low": return 1;
        case "medium": return 2;
        case "high": return 3;

        //Number evaluation
        default:
            var numbers = str.split("p")

            var value = parseInt(numbers[0])

            if (numbers.length > 1)
                value += parseInt(numbers[1])

            return value
        }
    }

    compareFunction: function(a, b) {
        return nameWeight(a) - nameWeight(b)
    }
}
