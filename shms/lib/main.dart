import 'package:flutter/material.dart';
import 'package:shms/env.dart';
import 'package:shms/water.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      theme: ThemeData.dark(),
      title: 'SHMS Dashboard',
      home: const MasterPage(),
    );
  }
}

class MasterPage extends StatefulWidget {
  const MasterPage({Key? key}) : super(key: key);
  @override
  State<StatefulWidget> createState() => _MasterPageState();
}

class _MasterPageState extends State<MasterPage> {
  late int _index;
  late DateTime date;

  @override
  void initState() {
    _index = 0;
    date = DateTime.now();
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Row(
        children: [
          NavigationRail(
              destinations: const [
                NavigationRailDestination(
                    icon: Icon(Icons.water_rounded),
                    label: Text("Water Consumption")),
                NavigationRailDestination(
                    icon: Icon(Icons.air), label: Text("Environtment")),
                NavigationRailDestination(
                    icon: Icon(Icons.date_range), label: Text("Select Date"))
              ],
              onDestinationSelected: (int index) async {
                if (index < 2) {
                  setState(() => _index = index);
                } else {
                  var tempDate = await showDatePicker(
                      context: context,
                      initialDate: DateTime.now(),
                      firstDate:
                          DateTime.now().subtract(const Duration(days: 365)),
                      lastDate: DateTime.now());
                  if (tempDate != null) {
                    setState(() => date = tempDate);
                  }
                }
              },
              selectedIndex: _index),
          const VerticalDivider(
            width: 1,
            thickness: 1,
          ),
          Expanded(
              child: _index == 0 ? WaterPage(date: date) : EnvPage(date: date)),
        ],
      ),
    );
  }
}
