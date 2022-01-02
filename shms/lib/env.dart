import 'package:flutter/material.dart';
import 'package:shms/http.dart';

class EnvPage extends StatefulWidget {
  final DateTime date;
  EnvPage({Key? key, required this.date}) : super(key: key);

  @override
  State<StatefulWidget> createState() => _EnvPageState();
}

class _EnvPageState extends State<EnvPage> {
  bool isLoading = true;
  bool isError = false;
  late EnvModelWrapper data;
  @override
  void initState() {
    getData();
    super.initState();
  }

  @override
  void didUpdateWidget(covariant EnvPage oldWidget) {
    getData();
    super.didUpdateWidget(oldWidget);
  }

  void getData() async {
    setState(() => isLoading = true);
    try {
      var tempData = await getEnvData(
          widget.date.year.toString(),
          widget.date.month.toString().padLeft(2, "0"),
          widget.date.day.toString().padLeft(2, "0"));
      setState(() {
        data = tempData;
        isError = false;
      });
    } catch (_) {
      setState(() => isError = true);
    }
    setState(() => isLoading = false);
  }

  @override
  Widget build(BuildContext context) {
    return isLoading
        ? const Center(
            child: CircularProgressIndicator(),
          )
        : isError
            ? Center(
                child: Column(
                  children: [
                    const Text("Something Went Wrong"),
                    ElevatedButton(
                        onPressed: getData, child: const Text("Retry"))
                  ],
                ),
              )
            : Padding(
                padding:
                    const EdgeInsets.symmetric(horizontal: 64, vertical: 32),
                child: Flex(
                  direction: Axis.vertical,
                  children: [
                    SizedBox(
                      width: double.infinity,
                      child: Card(
                        child: Container(
                          padding: const EdgeInsets.all(32),
                          child: Column(
                            children: [
                              const Text(
                                "Average Condition",
                                style: TextStyle(fontSize: 48),
                              ),
                              const SizedBox(
                                height: 8,
                              ),
                              Text(
                                "${data.aHum.toStringAsFixed(2)}% Humidity & ${data.aTemp.toStringAsFixed(2)} Celcius in Average",
                                style: const TextStyle(
                                    fontWeight: FontWeight.w700, fontSize: 32),
                              ),
                              const SizedBox(
                                height: 8,
                              ),
                              Text(
                                  "${widget.date.year}/${widget.date.month.toString().padLeft(2, "0")}/${widget.date.day.toString().padLeft(2, "0")}",
                                  style: const TextStyle(fontSize: 18))
                            ],
                          ),
                        ),
                      ),
                    ),
                    const SizedBox(
                      height: 16,
                    ),
                    Expanded(
                        child: ListView.separated(
                            itemBuilder: (c, i) => ListTile(
                                  title: Text(
                                      "${data.data[i].h} Humid & ${data.data[i].t} Celcius"),
                                  subtitle: Text(data.data[i].date),
                                  trailing: const Icon(Icons.cloud),
                                ),
                            separatorBuilder: (c, i) => const Divider(),
                            itemCount: data.data.length))
                  ],
                ),
              );
  }
}
