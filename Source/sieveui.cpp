//******************************************************************************
// Copyright 2020 ThirtySomething
//******************************************************************************
// This file is part of Sieve.
//
// Sieve is free software: you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// Sieve is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
// more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Sieve. If not, see <http://www.gnu.org/licenses/>.
//******************************************************************************

#include "ui_sieveui.h"
#include "sieveui.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <climits>
#include <thread>

// *****************************************************************************
// *****************************************************************************
SieveUI::SieveUI(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::SieveUI), m_processSieve()
{
    ui->setupUi(this);
    QObject::connect(this, &SieveUI::primeChanged, this, &SieveUI::setPrime);

    m_sieve = std::make_unique<net::derpaul::sieve::CSieve>(net::derpaul::sieve::CSieve::DEFAULT_SIEVE_SIZE);
    m_processSieve = std::future<void>();

    initQtElements();
}

// *****************************************************************************
// *****************************************************************************
SieveUI::~SieveUI()
{
    delete ui;
}

// *****************************************************************************
// *****************************************************************************
void SieveUI::on_actionAbout_Sieve_triggered(void)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("About Sieve");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText("<a href='https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes'>Sieve of Eratosthenes</a><br>(C) 2020 by <a href='https://github.com/ThirtySomething/Sieve'>ThirtySomething</a>");
    msgBox.exec();
}

// *****************************************************************************
// *****************************************************************************
void SieveUI::on_actionExport_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Export primes"), "primes.txt",
        tr("Primes (*.txt);;All Files (*)"));

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_sieve->exportPrimes(fileName.toStdString());
    QApplication::restoreOverrideCursor();
}

// *****************************************************************************
// *****************************************************************************
void SieveUI::on_actionLoad_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Load sieve data"), "sieve.dat",
        tr("Sieve data (*.dat);;All Files (*)"));

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_sieve->dataLoad(fileName.toStdString());
    ui->lblPrimeNumber->setText(QString::number(m_sieve->getLatestPrime()));
    ui->lblSieveSizeNumber->setText(QString::number(m_sieve->getSieveSize()));
    m_processSieve = std::future<void>();
    QApplication::restoreOverrideCursor();
}

// *****************************************************************************
// *****************************************************************************
void SieveUI::on_actionNew_triggered()
{
    bool ok;
    long long newSieveSize = QInputDialog::getInt(this, tr("Please enter sieve size"),
                                                  tr("Sieve size:"),
                                                  net::derpaul::sieve::CSieve::DEFAULT_SIEVE_SIZE,
                                                  LONG_MIN,
                                                  LONG_MAX,
                                                  1,
                                                  &ok);

    if (ok)
    {
        m_sieve = std::make_unique<net::derpaul::sieve::CSieve>(newSieveSize);
        ui->lblPrimeNumber->setText(QString::number(m_sieve->getLatestPrime()));
        ui->lblSieveSizeNumber->setText(QString::number(m_sieve->getSieveSize()));
        m_processSieve = std::future<void>();
    }
}

// *****************************************************************************
// *****************************************************************************
void SieveUI::on_actionQuit_triggered()
{
    QCoreApplication::quit();
}

// *****************************************************************************
// *****************************************************************************
void SieveUI::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save sieve data"), "sieve.dat",
        tr("Sieve data (*.dat);;All Files (*)"));

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_sieve->dataSave(fileName.toStdString());
    QApplication::restoreOverrideCursor();
}

// *****************************************************************************
// *****************************************************************************
void SieveUI::on_btnStart_clicked()
{
    if (m_processSieve.valid())
    {
        return;
    }

    m_processSieve = std::async(std::launch::async, [&]() {
        m_sieve->sievePrimes([&](long long currentPrime) {
            emit primeChanged(currentPrime);
        });
    });
    m_statusBar->showMessage("Sieving...");
}

// *****************************************************************************
// *****************************************************************************
void SieveUI::on_btnStop_clicked()
{
    m_sieve->interruptSieving();
    if (m_processSieve.valid())
    {
        m_processSieve.wait();
        m_processSieve = std::future<void>();
    }
    m_statusBar->showMessage("");
}

// *****************************************************************************
// *****************************************************************************
void SieveUI::setPrime(long long prime)
{
    ui->lblPrimeNumber->setText(QString::number(prime));
}

// *****************************************************************************
// *****************************************************************************
void SieveUI::initQtElements(void)
{
    ui->lblPrimeNumber->setText(QString::number(m_sieve->getLatestPrime()));
    ui->lblSieveSizeNumber->setText(QString::number(m_sieve->getSieveSize()));

    m_statusBar = SieveUI::statusBar();
}